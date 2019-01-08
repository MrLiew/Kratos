from __future__ import print_function, absolute_import, division  # makes KratosMultiphysics backward compatible with python 2.6 and 2.7

# Importing the Kratos Library
import KratosMultiphysics

# Check that applications were imported in the main script
KratosMultiphysics.CheckRegisteredApplications("FluidDynamicsApplication")

# Import applications
import KratosMultiphysics.FluidDynamicsApplication as KratosCFD

# Import base class file
from fluid_solver import FluidSolver

def CreateSolver(model, custom_settings):
    return NavierStokesTimeAveragedMonolithicSolver(model, custom_settings)

class NavierStokesTimeAveragedMonolithicSolver(FluidSolver):

    def _ValidateSettings(self, settings):
        ##settings string in json format
        default_settings = KratosMultiphysics.Parameters("""
        {
            "solver_type": "TimeAveraged",
            "model_part_name": "MainModelPart",
            "domain_size": -1,
            "model_import_settings": {
                "input_type": "mdpa",
                "input_filename": "unknown_name"
            },
            "maximum_iterations": 7,
            "predictor_corrector": true,
            "dynamic_tau": 1.0,
            "echo_level": 0,
            "time_order": 2,
            "compute_reactions": false,
            "reform_dofs_at_each_step": false,
            "relative_velocity_tolerance": 1e-3,
            "absolute_velocity_tolerance": 1e-5,
            "relative_pressure_tolerance": 1e-3,
            "absolute_pressure_tolerance": 1e-5,
            "linear_solver_settings"       : {
                "solver_type"         : "BICGSTABSolver",
                "max_iteration"       : 5000,
                "tolerance"           : 1e-7,
                "preconditioner_type" : "DiagonalPreconditioner",
                "scaling"             : false
            },
            "volume_model_part_name" : "volume_model_part",
            "skin_parts": [""],
            "no_skin_parts":[""],
            "time_stepping"                : {
                "automatic_time_step" : true,
                "CFL_number"          : 1,
                "minimum_delta_time"  : 1e-2,
                "maximum_delta_time"  : 1.0
            },
            "time_averaging_acceleration"        :{
                "considered_time"     : 5.0,
                "minimum_delta_time"  : 1e-2,
                "maximum_delta_time"  : 1.0
            },
            "periodic": "periodic",
            "move_mesh_flag": false
        }""")

        settings.ValidateAndAssignDefaults(default_settings)
        return settings

    def __init__(self, model, custom_settings):
        super(NavierStokesTimeAveragedMonolithicSolver,self).__init__(model,custom_settings)

        self.element_name = "TimeAveragedNavierStokes"
        self.condition_name = "TimeAveragedNavierStokesWallCondition"
        self.min_buffer_size = 5

        # There is only a single rank in OpenMP, we always print
        self._is_printing_rank = True

        ## Construct the linear solver
        import linear_solver_factory
        self.linear_solver = linear_solver_factory.ConstructSolver(self.settings["linear_solver_settings"])

        KratosMultiphysics.Logger.PrintInfo("NavierStokesTimeAveragedMonolithicSolver", "Construction of NavierStokesTimeAveragedMonolithicSolver finished.")


    def AddVariables(self):
        self.main_model_part.AddNodalSolutionStepVariable(KratosMultiphysics.DENSITY) # TODO: Remove this once the "old" embedded elements get the density from the properties (or once we delete them)
        self.main_model_part.AddNodalSolutionStepVariable(KratosMultiphysics.DYNAMIC_VISCOSITY) # TODO: Remove this once the "old" embedded elements get the density from the properties (or once we delete them)
        self.main_model_part.AddNodalSolutionStepVariable(KratosCFD.TIME_AVERAGED_PRESSURE)
        self.main_model_part.AddNodalSolutionStepVariable(KratosCFD.TIME_AVERAGED_VELOCITY)
        self.main_model_part.AddNodalSolutionStepVariable(KratosMultiphysics.ACCELERATION)
        self.main_model_part.AddNodalSolutionStepVariable(KratosMultiphysics.MESH_VELOCITY)
        self.main_model_part.AddNodalSolutionStepVariable(KratosMultiphysics.IS_STRUCTURE)
        self.main_model_part.AddNodalSolutionStepVariable(KratosMultiphysics.BODY_FORCE)
        self.main_model_part.AddNodalSolutionStepVariable(KratosMultiphysics.NODAL_H)
        self.main_model_part.AddNodalSolutionStepVariable(KratosMultiphysics.NODAL_AREA)
        self.main_model_part.AddNodalSolutionStepVariable(KratosMultiphysics.REACTION)
        self.main_model_part.AddNodalSolutionStepVariable(KratosMultiphysics.REACTION_WATER_PRESSURE)
        self.main_model_part.AddNodalSolutionStepVariable(KratosMultiphysics.NORMAL)
        self.main_model_part.AddNodalSolutionStepVariable(KratosMultiphysics.EXTERNAL_PRESSURE)

        KratosMultiphysics.Logger.PrintInfo("NavierStokesTimeAveragedMonolithicSolver", "Fluid solver variables added correctly.")


    def AddDofs(self):
        KratosMultiphysics.VariableUtils().AddDof(KratosCFD.TIME_AVERAGED_VELOCITY_X, KratosMultiphysics.REACTION_X,              self.main_model_part)
        KratosMultiphysics.VariableUtils().AddDof(KratosCFD.TIME_AVERAGED_VELOCITY_Y, KratosMultiphysics.REACTION_Y,              self.main_model_part)
        KratosMultiphysics.VariableUtils().AddDof(KratosCFD.TIME_AVERAGED_VELOCITY_Z, KratosMultiphysics.REACTION_Z,              self.main_model_part)
        KratosMultiphysics.VariableUtils().AddDof(KratosCFD.TIME_AVERAGED_PRESSURE,   KratosMultiphysics.REACTION_WATER_PRESSURE, self.main_model_part)

        if self._IsPrintingRank():
            KratosMultiphysics.Logger.PrintInfo("NavierStokesTimeAveragedMonolithicSolver", "Fluid solver DOFs added correctly.")


    def ImportModelPart(self):
        super(NavierStokesTimeAveragedMonolithicSolver, self).ImportModelPart()


    def PrepareModelPart(self):
        super(NavierStokesTimeAveragedMonolithicSolver, self).PrepareModelPart()
        if not self.main_model_part.ProcessInfo[KratosMultiphysics.IS_RESTARTED]:
            ## Sets DENSITY, DYNAMIC_VISCOSITY and SOUND_VELOCITY
            self._set_physical_properties()
            ## Sets the constitutive law
            self._set_constitutive_law()
            ## Sets averaging time length
            self._set_averaging_time_length()


    def Initialize(self):
        self.computing_model_part = self.GetComputingModelPart()

        # If needed, create the estimate time step utility
        if (self.settings["time_stepping"]["automatic_time_step"].GetBool()):
            self.EstimateDeltaTimeUtility = self._GetAutomaticTimeSteppingUtility()

        # Creating the solution strategy
        self.conv_criteria = KratosCFD.VelPrCriteria(self.settings["relative_velocity_tolerance"].GetDouble(),
                                                     self.settings["absolute_velocity_tolerance"].GetDouble(),
                                                     self.settings["relative_pressure_tolerance"].GetDouble(),
                                                     self.settings["absolute_pressure_tolerance"].GetDouble())

        (self.conv_criteria).SetEchoLevel(self.settings["echo_level"].GetInt())

        self.bdf_process = KratosMultiphysics.ComputeBDFCoefficientsProcess(self.computing_model_part,
                                                                            self.settings["time_order"].GetInt())

        time_scheme = KratosMultiphysics.ResidualBasedIncrementalUpdateStaticSchemeSlip(self.main_model_part.ProcessInfo[KratosMultiphysics.DOMAIN_SIZE],   # Domain size (2,3)
                                                                                        self.main_model_part.ProcessInfo[KratosMultiphysics.DOMAIN_SIZE]+1) # DOFs (3,4)

        builder_and_solver = KratosMultiphysics.ResidualBasedBlockBuilderAndSolver(self.linear_solver)

        self.solver = KratosMultiphysics.ResidualBasedNewtonRaphsonStrategy(self.computing_model_part,
                                                                            time_scheme,
                                                                            self.linear_solver,
                                                                            self.conv_criteria,
                                                                            builder_and_solver,
                                                                            self.settings["maximum_iterations"].GetInt(),
                                                                            self.settings["compute_reactions"].GetBool(),
                                                                            self.settings["reform_dofs_at_each_step"].GetBool(),
                                                                            self.settings["move_mesh_flag"].GetBool())

        (self.solver).SetEchoLevel(self.settings["echo_level"].GetInt())

        (self.solver).Initialize() # Initialize the solver. Otherwise the constitutive law is not initializated.
        (self.solver).Check()
        self.main_model_part.ProcessInfo.SetValue(KratosMultiphysics.DYNAMIC_TAU, self.settings["dynamic_tau"].GetDouble())

        KratosMultiphysics.Logger.PrintInfo("NavierStokesTimeAveragedMonolithicSolver", "Solver initialization finished.")


    def AdvanceInTime(self, current_time):
        #dt = self._ComputeDeltaTime()
        current_dt = self.main_model_part.ProcessInfo[KratosMultiphysics.DELTA_TIME]
        dt = self._ComputeIncreasedDt(current_dt)
        new_time = current_time + dt

        self.main_model_part.CloneTimeStep(new_time)
        self.main_model_part.ProcessInfo[KratosMultiphysics.STEP] += 1

        averaging_time_length = self.settings["time_averaging_acceleration"]["considered_time"].GetDouble()
        if new_time > averaging_time_length:
            print("Averaging time length set to ", averaging_time_length, ", droping previous time infomation")

        return new_time


    def Solve(self):
        self.InitializeSolutionStep()
        self.Predict()
        self.SolveSolutionStep()
        self.FinalizeSolutionStep()


    def InitializeSolutionStep(self):
        if self._TimeBufferIsInitialized():
            (self.bdf_process).Execute()
            (self.solver).InitializeSolutionStep()


    def _set_physical_properties(self):
        ## Set the SOUND_VELOCITY value (wave velocity)
        if self.main_model_part.Properties[1].Has(KratosMultiphysics.SOUND_VELOCITY):
            self.main_model_part.ProcessInfo[KratosMultiphysics.SOUND_VELOCITY] = self.main_model_part.Properties[1][KratosMultiphysics.SOUND_VELOCITY]
        else:
            # If the wave velocity is not defined take a large enough value to consider the fluid as incompressible
            default_sound_velocity = 1e+12
            self.main_model_part.ProcessInfo[KratosMultiphysics.SOUND_VELOCITY] = default_sound_velocity

        # Transfer density and (dynamic) viscostity to the nodes
        for el in self.main_model_part.Elements:
            rho = el.Properties.GetValue(KratosMultiphysics.DENSITY)
            if rho <= 0.0:
                raise Exception("DENSITY set to {0} in Properties {1}, positive number expected.".format(rho,el.Properties.Id))
            dyn_viscosity = el.Properties.GetValue(KratosMultiphysics.DYNAMIC_VISCOSITY)
            if dyn_viscosity <= 0.0:
                raise Exception("DYNAMIC_VISCOSITY set to {0} in Properties {1}, positive number expected.".format(dyn_viscosity,el.Properties.Id))
            break

        # TODO: Remove this once the "old" embedded elements get the density from the properties (or once we delete them)
        KratosMultiphysics.VariableUtils().SetScalarVar(KratosMultiphysics.DENSITY, rho, self.main_model_part.Nodes)
        KratosMultiphysics.VariableUtils().SetScalarVar(KratosMultiphysics.DYNAMIC_VISCOSITY, dyn_viscosity, self.main_model_part.Nodes)


    def _ComputeIncreasedDt(self, current_dt):
        dt_min = self.settings["time_averaging_acceleration"]["minimum_delta_time"].GetDouble()
        dt_max = self.settings["time_averaging_acceleration"]["maximum_delta_time"].GetDouble()
        new_dt = current_dt + 0.01
        if (new_dt >= dt_max):
            new_dt = dt_max
        print("New dt is: ", new_dt)
        return new_dt


    def _set_averaging_time_length(self):
        averaging_time_length = self.settings["time_averaging_acceleration"]["considered_time"].GetDouble()
        self.main_model_part.ProcessInfo.SetValue(KratosCFD.AVERAGING_TIME_LENGTH, averaging_time_length)


    def _set_constitutive_law(self):
        ## Construct the constitutive law needed for the embedded element
        if(self.main_model_part.ProcessInfo[KratosMultiphysics.DOMAIN_SIZE] == 3):
            self.main_model_part.Properties[1][KratosMultiphysics.CONSTITUTIVE_LAW] = KratosCFD.Newtonian3DLaw()
        elif(self.main_model_part.ProcessInfo[KratosMultiphysics.DOMAIN_SIZE] == 2):
            self.main_model_part.Properties[1][KratosMultiphysics.CONSTITUTIVE_LAW] = KratosCFD.Newtonian2DLaw()