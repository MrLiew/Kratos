from __future__ import print_function, absolute_import, division
import KratosMultiphysics
import os
import KratosMultiphysics.KratosUnittest as KratosUnittest
from KratosMultiphysics.StructuralMechanicsApplication.structural_mechanics_analysis import StructuralMechanicsAnalysis
import KratosMultiphysics.StructuralMechanicsApplication as StructuralMechanicsApplication
import structural_response_function_factory
import KratosMultiphysics.kratos_utilities as kratos_utils

class AdjointSensitivityNonlinearTruss(KratosUnittest.TestCase):
    def _removeH5Files(self, model_part_name):
        for name in os.listdir():
            if name.find(model_part_name) == 0:
                kratos_utils.DeleteFileIfExisting(name)

    def testStructureWithFreeNodes(self):
        with open("response_function_tests/adjoint_strain_energy_response_parameters_truss_free_nodes.json",'r') as parameter_file:
            parameters = KratosMultiphysics.Parameters( parameter_file.read())

        model = KratosMultiphysics.Model()
        response_function = structural_response_function_factory.CreateResponseFunction("dummy", parameters["kratos_response_settings"], model)
        model_part_adjoint = response_function.adjoint_model_part
        response_function.RunCalculation(calculate_gradient=True)

        node_sensitivity = []
        node_adjoint_displacement = []
        node_primal_displacement = []
        for i in range(1,4):
            node_sensitivity.append( model_part_adjoint.GetNode(i).GetSolutionStepValue(KratosMultiphysics.SHAPE_SENSITIVITY) )
            node_adjoint_displacement.append( model_part_adjoint.GetNode(i).GetSolutionStepValue(StructuralMechanicsApplication.ADJOINT_DISPLACEMENT) )
            node_primal_displacement.append( model_part_adjoint.GetNode(i).GetSolutionStepValue(KratosMultiphysics.DISPLACEMENT) )

        ## Testing Primal Displacement
        self.assertAlmostEqual(node_primal_displacement[0][1], -0.380132, 5)
        self.assertAlmostEqual(node_primal_displacement[1][0], -0.0183309, 5)
        self.assertAlmostEqual(node_primal_displacement[1][1], -0.152731, 5)

        ## Testing Adjoint Displacement
        self.assertAlmostEqual(node_adjoint_displacement[0][1], -0.592063, 5)
        self.assertAlmostEqual(node_adjoint_displacement[1][0], -0.0377389, 5)
        self.assertAlmostEqual(node_adjoint_displacement[1][1], -0.187154, 5)

        # ## Testing Adjoint sensitivity values with Finite difference results
        self.assertAlmostEqual(node_sensitivity[0][0], 0.00045709764862067454, 2)
        self.assertAlmostEqual(node_sensitivity[0][1], -6.556528466905575, 1)
        self.assertAlmostEqual(node_sensitivity[1][0], -2.9015045561742165, 1)
        self.assertAlmostEqual(node_sensitivity[1][1], 2.6497931858493473, 1)
        self.assertAlmostEqual(node_sensitivity[2][0], -1.57271274847659, 2)
        self.assertAlmostEqual(node_sensitivity[2][1], 0.6297830440260554, 3)

        ## Testing the Implementation
        self.assertAlmostEqual(node_sensitivity[0][0], -9.115050795149848e-05, 2)
        self.assertAlmostEqual(node_sensitivity[0][1], -6.531888931421271, 3)
        self.assertAlmostEqual(node_sensitivity[1][0], -2.890008379474549, 3)
        self.assertAlmostEqual(node_sensitivity[1][1], 2.636448174422331, 3)
        self.assertAlmostEqual(node_sensitivity[2][0], -1.5717646080575238, 3)
        self.assertAlmostEqual(node_sensitivity[2][1], 0.6295943357470097, 3)

        self._removeH5Files("primal_output_truss")

    def testStructureWithMultipleLoads(self):
        with open("response_function_tests/adjoint_strain_energy_response_parameters_truss_multiple_loads.json",'r') as parameter_file:
            parameters = KratosMultiphysics.Parameters( parameter_file.read())

        model = KratosMultiphysics.Model()
        response_function = structural_response_function_factory.CreateResponseFunction("dummy", parameters["kratos_response_settings"], model)
        model_part_adjoint = response_function.adjoint_model_part
        response_function.RunCalculation(calculate_gradient=True)

        node_sensitivity = []
        node_adjoint_displacement = []
        node_primal_displacement = []
        for i in range(1,4):
            node_sensitivity.append( model_part_adjoint.GetNode(i).GetSolutionStepValue(KratosMultiphysics.SHAPE_SENSITIVITY) )
            node_adjoint_displacement.append( model_part_adjoint.GetNode(i).GetSolutionStepValue(StructuralMechanicsApplication.ADJOINT_DISPLACEMENT) )
            node_primal_displacement.append( model_part_adjoint.GetNode(i).GetSolutionStepValue(KratosMultiphysics.DISPLACEMENT) )

        ## Testing Primal Displacement
        self.assertAlmostEqual(node_primal_displacement[0][1], -0.250272, 5)
        self.assertAlmostEqual(node_primal_displacement[1][0], 0.0104822, 5)
        self.assertAlmostEqual(node_primal_displacement[1][1], -0.182561, 5)

        ## Testing Adjoint Displacement
        self.assertAlmostEqual(node_adjoint_displacement[0][1], -0.334002, 5)
        self.assertAlmostEqual(node_adjoint_displacement[1][0], 0.0199123, 5)
        self.assertAlmostEqual(node_adjoint_displacement[1][1], -0.270252, 5)

        # ## Testing Adjoint sensitivity values with Finite difference results
        self.assertAlmostEqual(node_sensitivity[0][0],-0.056284024907427004, 0)
        self.assertAlmostEqual(node_sensitivity[0][1], -0.056284024907427004, 3)
        self.assertAlmostEqual(node_sensitivity[1][0], 1.932646787583536, 2)
        self.assertAlmostEqual(node_sensitivity[1][1], -1.9382867385342448, 2)
        self.assertAlmostEqual(node_sensitivity[2][0], -2.5592816343511515, 2)
        self.assertAlmostEqual(node_sensitivity[2][1], 1.9672906711676317, 1)

        ## Testing the Implementation
        self.assertAlmostEqual(node_sensitivity[0][0], -1.8556330045951808e-06, 2)
        self.assertAlmostEqual(node_sensitivity[0][1], -0.05663008009911687, 3)
        self.assertAlmostEqual(node_sensitivity[1][0],  1.9280933541049419, 3)
        self.assertAlmostEqual(node_sensitivity[1][1],  -1.9338276499154738, 3)
        self.assertAlmostEqual(node_sensitivity[2][0], -2.5545319490490432, 3)
        self.assertAlmostEqual(node_sensitivity[2][1], 1.9621841224837615, 3)

        self._removeH5Files("primal_output_truss")

    def testThreeDimensionalStructure(self):
        with open("response_function_tests/adjoint_strain_energy_response_parameters_3D_truss.json",'r') as parameter_file:
            parameters = KratosMultiphysics.Parameters( parameter_file.read())

        model = KratosMultiphysics.Model()
        response_function = structural_response_function_factory.CreateResponseFunction("dummy", parameters["kratos_response_settings"], model)
        model_part_adjoint = response_function.adjoint_model_part
        response_function.RunCalculation(calculate_gradient=True)

        node_sensitivity = []
        node_adjoint_displacement = []
        node_primal_displacement = []
        for i in range(1,4):
            node_sensitivity.append( model_part_adjoint.GetNode(i).GetSolutionStepValue(KratosMultiphysics.SHAPE_SENSITIVITY) )
            node_adjoint_displacement.append( model_part_adjoint.GetNode(i).GetSolutionStepValue(StructuralMechanicsApplication.ADJOINT_DISPLACEMENT) )
            node_primal_displacement.append( model_part_adjoint.GetNode(i).GetSolutionStepValue(KratosMultiphysics.DISPLACEMENT) )

        ## Testing Primal Displacement
        self.assertAlmostEqual(node_primal_displacement[0][2], -1.31138, 5)

        ## Testing Adjoint Displacement
        self.assertAlmostEqual(node_adjoint_displacement[0][2], -2.60392, 5)

        ## Testing Adjoint sensitivity values with Finite difference results
        self.assertAlmostEqual(node_sensitivity[0][0], 0.003973418927216699, 2)
        self.assertAlmostEqual(node_sensitivity[0][1], 0.00012844054708693875, 2)
        self.assertAlmostEqual(node_sensitivity[0][2], 12.575784792545617, 2)
        self.assertAlmostEqual(node_sensitivity[1][0], -6.742208012155969, 1)
        self.assertAlmostEqual(node_sensitivity[1][1], -3.371206929614345, 1)
        self.assertAlmostEqual(node_sensitivity[1][2], -3.143893783885687, 2)
        self.assertAlmostEqual(node_sensitivity[2][0], 6.742758621669508, 1)
        self.assertAlmostEqual(node_sensitivity[2][1], -3.3712069225089176, 1)
        self.assertAlmostEqual(node_sensitivity[2][2], -3.143893783885687, 2)

        ## Testing the Implementation
        self.assertAlmostEqual(node_sensitivity[0][0], 0.00040798663816898895, 3)
        self.assertAlmostEqual(node_sensitivity[0][1], 0.0004700215574438005, 3)
        self.assertAlmostEqual(node_sensitivity[0][2], 12.574373868481054, 3)
        self.assertAlmostEqual(node_sensitivity[1][0], -6.725838536707082, 3)
        self.assertAlmostEqual(node_sensitivity[1][1], -3.362850508101536, 3)
        self.assertAlmostEqual(node_sensitivity[1][2],  -3.1435231573381937, 3)
        self.assertAlmostEqual(node_sensitivity[2][0], 6.726034062602099, 3)
        self.assertAlmostEqual(node_sensitivity[2][1], -3.36285078125232, 3)
        self.assertAlmostEqual(node_sensitivity[2][2], -3.1435248382244194, 3)

        self._removeH5Files("primal_output_truss")

    def testMisesTrussStructure(self):
        with open("response_function_tests/adjoint_strain_energy_response_parameters_truss.json",'r') as parameter_file:
            parameters = KratosMultiphysics.Parameters( parameter_file.read())

        model = KratosMultiphysics.Model()
        response_function = structural_response_function_factory.CreateResponseFunction("dummy", parameters["kratos_response_settings"], model)
        model_part_adjoint = response_function.adjoint_model_part
        response_function.RunCalculation(calculate_gradient=True)

        node_sensitivity = []
        node_adjoint_displacement = []
        node_primal_displacement = []
        for i in range(1,4):
            node_sensitivity.append( model_part_adjoint.GetNode(i).GetSolutionStepValue(KratosMultiphysics.SHAPE_SENSITIVITY) )
            node_adjoint_displacement.append( model_part_adjoint.GetNode(i).GetSolutionStepValue(StructuralMechanicsApplication.ADJOINT_DISPLACEMENT) )
            node_primal_displacement.append( model_part_adjoint.GetNode(i).GetSolutionStepValue(KratosMultiphysics.DISPLACEMENT) )

        ## Testing Primal Displacement
        self.assertAlmostEqual(node_primal_displacement[0][1], -0.284834, 5)

        ## Testing Adjoint Displacement
        self.assertAlmostEqual(node_adjoint_displacement[0][1], -0.647269, 5)

        # ## Testing Adjoint sensitivity values with Finite difference results
        self.assertAlmostEqual(node_sensitivity[0][0],0.196686219489095, 0)
        self.assertAlmostEqual(node_sensitivity[0][1], -5.9577956088574515, 1)
        self.assertAlmostEqual(node_sensitivity[1][0], -14.449074176425823, 1)
        self.assertAlmostEqual(node_sensitivity[1][1], 3.0951781273103047, 0)

        ## Testing the Implementation
        self.assertAlmostEqual(node_sensitivity[0][0],-6.1793270740295725e-06, 2)
        self.assertAlmostEqual(node_sensitivity[0][1], -5.948676601952164, 3)
        self.assertAlmostEqual(node_sensitivity[1][0], -14.48698511892927, 3)
        self.assertAlmostEqual(node_sensitivity[1][1], 2.974545352421102, 3)

        self._removeH5Files("primal_output_truss")

## Most of the time required for the test is used in reading and wrting out file
if __name__ == '__main__':
    suites = KratosUnittest.KratosSuites
    nightSuite = suites['nightly']
    nightSuite.addTests(KratosUnittest.TestLoader().loadTestsFromTestCases([AdjointSensitivityNonlinearTruss]))
    KratosUnittest.runTests(suites)
    allSuite = suites['all']
    allSuite.addTests(nightSuite)
    KratosUnittest.runTests(suites)