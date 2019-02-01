//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    @{KRATOS_APP_AUTHOR}
//

#if !defined(KRATOS_RANS_CONSTITUTIVE_LAWS_APPLICATION_VARIABLES_H_INCLUDED )
#define  KRATOS_RANS_CONSTITUTIVE_LAWS_APPLICATION_VARIABLES_H_INCLUDED

// System includes

// External includes

// Project includes
#include "includes/define.h"
#include "includes/variables.h"
#include "includes/kratos_application.h"

namespace Kratos
{
KRATOS_DEFINE_VARIABLE( double, TURBULENT_KINETIC_ENERGY )
KRATOS_DEFINE_VARIABLE( double, TURBULENT_ENERGY_DISSIPATION_RATE )

}

#endif	/* KRATOS_RANS_CONSTITUTIVE_LAWS_APPLICATION_VARIABLES_H_INCLUDED */
