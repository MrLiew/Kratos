//
//   Project Name:        KratosConstitutiveModelsApplication $
//   Created by:          $Author:                JMCarbonell $
//   Last modified by:    $Co-Author:                         $
//   Date:                $Date:                   April 2017 $
//   Revision:            $Revision:                      0.0 $
//
//

#include "constitutive_models_application_variables.h"

namespace Kratos
{

  //specific constitutive models variables must be CREATED here
  KRATOS_CREATE_VARIABLE(std::string, TEMPERATURE_VARIABLE)
  KRATOS_CREATE_VARIABLE(std::string, PRESSURE_VARIABLE)

  KRATOS_CREATE_VARIABLE(PropertiesLayout, PROPERTIES_LAYOUT)

  
   KRATOS_CREATE_VARIABLE( double, RHOS )   
   KRATOS_CREATE_VARIABLE( double, RHOT )   
   KRATOS_CREATE_VARIABLE( double, CHIS )   
   KRATOS_CREATE_VARIABLE( double, CHIT )   
   KRATOS_CREATE_VARIABLE( double, REFERENCE_PRESSURE )   

   KRATOS_CREATE_VARIABLE( double, SPACING_RATIO )   
   KRATOS_CREATE_VARIABLE( double, SHAPE_PARAMETER )  
   KRATOS_CREATE_VARIABLE( double, P0 )  
   KRATOS_CREATE_VARIABLE( double, PC )  
   KRATOS_CREATE_VARIABLE( double, BONDING )
   KRATOS_CREATE_VARIABLE( double, INITIAL_BONDING )
   KRATOS_CREATE_VARIABLE( double, DEGRADATION_THRESHOLD )
   KRATOS_CREATE_VARIABLE( double, DEGRADATION_RATE_COMPRESSION )
   KRATOS_CREATE_VARIABLE( double, DEGRADATION_RATE_SHEAR )
   KRATOS_CREATE_VARIABLE( double, ALPHA_TENSILE )
   KRATOS_CREATE_VARIABLE( double, PLASTIC_DEVIATORIC_STRAIN_HARDENING )

   KRATOS_CREATE_VARIABLE( double, PLASTIC_VOL_DEF )   
   KRATOS_CREATE_VARIABLE( double, NONLOCAL_PLASTIC_VOL_DEF )   
   KRATOS_CREATE_VARIABLE( double, PLASTIC_VOL_DEF_ABS )   
   KRATOS_CREATE_VARIABLE( double, NONLOCAL_PLASTIC_VOL_DEF_ABS )   
   KRATOS_CREATE_VARIABLE( double, PLASTIC_DEV_DEF )   
   KRATOS_CREATE_VARIABLE( double, NONLOCAL_PLASTIC_DEV_DEF )   

   KRATOS_CREATE_VARIABLE( double, KSIM )   
   KRATOS_CREATE_VARIABLE( double, PS )   
   KRATOS_CREATE_VARIABLE( double, PT )   
   KRATOS_CREATE_VARIABLE( double, PM )  

   KRATOS_CREATE_VARIABLE( double, COHESION )   
}
