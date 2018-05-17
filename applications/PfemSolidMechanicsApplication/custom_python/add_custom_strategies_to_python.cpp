//
//   Project Name:        KratosPfemSolidMechanicsApplication $
//   Created by:          $Author:                JMCarbonell $
//   Last modified by:    $Co-Author:                         $
//   Date:                $Date:                    July 2013 $
//   Revision:            $Revision:                      0.0 $
//
//

// System includes 

// External includes 

// Project includes
#include "spaces/ublas_space.h"
#include "custom_python/add_custom_strategies_to_python.h"

// Schemes
#include "custom_strategies/schemes/residual_based_bossak_scheme.hpp"
#include "custom_strategies/schemes/residual_based_U_W_bossak_scheme.hpp"

namespace Kratos
{

namespace Python
{		
using namespace  pybind11;

void  AddCustomStrategiesToPython(pybind11::module& m)
{
  typedef UblasSpace<double, CompressedMatrix, Vector>                         SparseSpaceType;
  typedef UblasSpace<double, Matrix, Vector>                                    LocalSpaceType;
  typedef Scheme< SparseSpaceType, LocalSpaceType >                                 SchemeType;
  typedef SolutionScheme<SparseSpaceType, LocalSpaceType>                   SolutionSchemeType;

  //custom scheme types
  typedef ResidualBasedBossakScheme< SparseSpaceType, LocalSpaceType >              ResidualBasedBossakSchemeType;
  typedef ResidualBasedUWBossakScheme< SparseSpaceType, LocalSpaceType >          ResidualBasedUWBossakSchemeType;

      
  //*************************SHCHEME CLASSES****************************

     
  // Residual Based Bossak Scheme Type
  class_<ResidualBasedBossakSchemeType, typename ResidualBasedBossakSchemeType::Pointer, SchemeType>
      (m,"ResidualBasedBossakScheme")
      .def(init< double , double >())
      .def("Initialize", &ResidualBasedBossakScheme<SparseSpaceType, LocalSpaceType>::Initialize)
      ;
      

  // Residual Based Bossak Scheme Type
  class_<ResidualBasedUWBossakSchemeType, typename ResidualBasedUWBossakSchemeType::Pointer, SolutionSchemeType>
      (m,"ResidualBasedUWBossakScheme")
      .def(init<>())
      .def(init<Flags&>())
      ;
                     
}

}  // namespace Python.

} // Namespace Kratos

