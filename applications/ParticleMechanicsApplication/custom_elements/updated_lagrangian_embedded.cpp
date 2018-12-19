//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		BSD License
//					Kratos default license: kratos/license.txt
//
//  Main authors:   Bodhinanda Chandra
//


// System includes
#include <omp.h>
#include <sstream>

// External includes

// Project includes
#include "includes/define.h"
#include "includes/kratos_flags.h"
#include "custom_elements/updated_lagrangian_embedded.hpp"
#include "utilities/math_utils.h"
#include "includes/constitutive_law.h"
#include "custom_utilities/particle_mechanics_math_utilities.h"
#include "particle_mechanics_application_variables.h"

namespace Kratos
{

//******************************CONSTRUCTOR*******************************************
//************************************************************************************

UpdatedLagrangianEmbedded::UpdatedLagrangianEmbedded( )
    : UpdatedLagrangian( )
{
    //DO NOT CALL IT: only needed for Register and Serialization!!!
}

//******************************CONSTRUCTOR*******************************************
//************************************************************************************

UpdatedLagrangianEmbedded::UpdatedLagrangianEmbedded( IndexType NewId, GeometryType::Pointer pGeometry )
        : UpdatedLagrangian( NewId, pGeometry )
{
    //DO NOT ADD DOFS HERE!!!
}

//******************************CONSTRUCTOR*******************************************
//************************************************************************************

UpdatedLagrangianEmbedded::UpdatedLagrangianEmbedded( IndexType NewId, GeometryType::Pointer pGeometry, PropertiesType::Pointer pProperties )
        : UpdatedLagrangian( NewId, pGeometry, pProperties )
{
mFinalizedStep = true;
}

//******************************COPY CONSTRUCTOR**************************************
//************************************************************************************

UpdatedLagrangianEmbedded::UpdatedLagrangianEmbedded( UpdatedLagrangianEmbedded const& rOther)
    :UpdatedLagrangian(rOther)
{
}

//*********************************OPERATIONS*****************************************
//************************************************************************************

Element::Pointer UpdatedLagrangianEmbedded::Create( IndexType NewId, NodesArrayType const& ThisNodes, PropertiesType::Pointer pProperties ) const
{
    return Element::Pointer( new UpdatedLagrangianEmbedded( NewId, GetGeometry().Create( ThisNodes ), pProperties ) );
}

//************************************CLONE*******************************************
//************************************************************************************

Element::Pointer UpdatedLagrangianEmbedded::Clone( IndexType NewId, NodesArrayType const& rThisNodes ) const
{

    UpdatedLagrangianEmbedded NewElement (NewId, GetGeometry().Create( rThisNodes ), pGetProperties() );

    return Element::Pointer( new UpdatedLagrangianEmbedded(NewElement) );
}

//*******************************DESTRUCTOR*******************************************
//************************************************************************************

UpdatedLagrangianEmbedded::~UpdatedLagrangianEmbedded()
{
}

//************************************************************************************
//************************************************************************************

void UpdatedLagrangianEmbedded::CalculateElementalSystem( LocalSystemComponents& rLocalSystem,
        ProcessInfo& rCurrentProcessInfo)
{
    KRATOS_TRY

    // Initialize embedded variables
    this->InitializeEmbeddedVariables();

    // Create and initialize element variables (considering whether it is cut or not):
    GeneralVariables Variables;
    this->InitializeGeneralVariables(Variables,rCurrentProcessInfo);

    // Create constitutive law parameters:
    ConstitutiveLaw::Parameters Values(GetGeometry(),GetProperties(),rCurrentProcessInfo);

    // Set constitutive law flags:
    Flags &ConstitutiveLawOptions=Values.GetOptions();
    ConstitutiveLawOptions.Set(ConstitutiveLaw::USE_ELEMENT_PROVIDED_STRAIN);
    ConstitutiveLawOptions.Set(ConstitutiveLaw::COMPUTE_STRESS);
    ConstitutiveLawOptions.Set(ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR);

    // Auxiliary terms
    Vector volume_force;

    // Compute element kinematics B, F, DN_DX ...
    this->CalculateKinematics(Variables,rCurrentProcessInfo);

    // Set general variables to constitutivelaw parameters
    this->SetGeneralVariables(Variables,Values);

    // Calculate Material Response
    /* NOTE:
    The function below will call CalculateMaterialResponseCauchy() by default and then (may)
    call CalculateMaterialResponseKirchhoff() in the constitutive_law.*/
    mConstitutiveLawVector->CalculateMaterialResponse(Values, Variables.StressMeasure);

    /* NOTE:
    The material points will have constant mass as defined at the beginning.
    However, the density and volume (integration weight) are changing every time step.*/
    // Update MP_Density
    const double MP_Density = (GetProperties()[DENSITY]) / Variables.detFT;
    this->SetValue(MP_DENSITY, MP_Density);

    // The MP_Volume (integration weight) is evaluated
    const double MP_Volume = this->GetValue(MP_MASS)/this->GetValue(MP_DENSITY);
    this->SetValue(MP_VOLUME, MP_Volume);

    if ( rLocalSystem.CalculationFlags.Is(UpdatedLagrangian::COMPUTE_LHS_MATRIX) ) // if calculation of the matrix is required
    {
        // Contributions to stiffness matrix calculated on the reference configuration
        this->CalculateAndAddLHS ( rLocalSystem, Variables, MP_Volume );
    }

    if ( rLocalSystem.CalculationFlags.Is(UpdatedLagrangian::COMPUTE_RHS_VECTOR) ) // if calculation of the vector is required
    {
        // Contribution to forces (in residual term) are calculated
        volume_force  = this->CalculateVolumeForce( volume_force, Variables );
        this->CalculateAndAddRHS ( rLocalSystem, Variables, volume_force, MP_Volume );
    }

    KRATOS_CATCH( "" )
}

//************************************************************************************
//************************************************************************************

void UpdatedLagrangianEmbedded::InitializeEmbeddedVariables()
{
    const GeometryType& rGeom = GetGeometry();
    const unsigned int number_of_nodes = GetGeometry().size();

    // Initialize mDistance as from obtained DISTANCE variables
    if (mDistance.size() != number_of_nodes)
        mDistance.resize(number_of_nodes, false);
    mDistance = ZeroVector(number_of_nodes);

    for (size_t i = 0; i < number_of_nodes; i++)
        mDistance[i] = rGeom[i].FastGetSolutionStepValue(DISTANCE);

    // Check whether the element is a SLIP element or not
    mIsSlip = this->Is(SLIP) ? true : false;

    // Clear previous positive and negative number of nodes and indices
    mNumPositiveNodes = 0;
    mNumNegativeNodes = 0;
    mPositiveIndices.clear();
    mNegativeIndices.clear();

    // Number of positive and negative distance function values
    for (size_t i = 0; i < number_of_nodes; ++i){
        // For positive distance
        if (mDistance[i] > 0.0) {
            mNumPositiveNodes++;
            mPositiveIndices.push_back(i);
        }
        // For negative distance
        else {
            mNumNegativeNodes++;
            mNegativeIndices.push_back(i);
        }
    }
}

//************************************************************************************
//************************************************************************************

void UpdatedLagrangianEmbedded::InitializeGeneralVariables (GeneralVariables& rVariables, const ProcessInfo& rCurrentProcessInfo)
{
    if (!this->IsCut()){
        UpdatedLagrangian::InitializeGeneralVariables(rVariables, rCurrentProcessInfo);
    }
    else{
        const unsigned int number_of_nodes = GetGeometry().size();
        const unsigned int dimension = GetGeometry().WorkingSpaceDimension();
        unsigned int voigtsize  = 3;

        if( dimension == 3 )
        {
            voigtsize  = 6;
        }
        rVariables.detF  = 1;

        rVariables.detF0 = 1;

        rVariables.detFT = 1;

        rVariables.detJ = 1;

        rVariables.B.resize( voigtsize, number_of_nodes * dimension, false );

        rVariables.F.resize( dimension, dimension, false );

        rVariables.F0.resize( dimension, dimension, false );

        rVariables.FT.resize( dimension, dimension, false );

        rVariables.ConstitutiveMatrix.resize( voigtsize, voigtsize, false );

        rVariables.StrainVector.resize( voigtsize, false );

        rVariables.StressVector.resize( voigtsize, false );

        rVariables.DN_DX.resize( number_of_nodes, dimension, false );
        rVariables.DN_De.resize( number_of_nodes, dimension, false );

        const array_1d<double,3>& xg = this->GetValue(MP_COORD);

        rVariables.N = this->MPMShapeFunctionPointValues(rVariables.N, xg);

        // Reading shape functions local gradients
        rVariables.DN_De = this->MPMShapeFunctionsLocalGradients( rVariables.DN_De);

        // CurrentDisp is the unknown variable. It represents the nodal delta displacement. When it is predicted is equal to zero.
        rVariables.CurrentDisp = CalculateCurrentDisp(rVariables.CurrentDisp, rCurrentProcessInfo);

        // Calculating the current jacobian from cartesian coordinates to parent coordinates for the MP element [dx_n+1/d£]
        rVariables.j = this->MPMJacobianDelta( rVariables.j, xg, rVariables.CurrentDisp);

        // Calculating the reference jacobian from cartesian coordinates to parent coordinates for the MP element [dx_n/d£]
        rVariables.J = this->MPMJacobian( rVariables.J, xg);
    }
}

//************************************************************************************
//************************************************************************************

void UpdatedLagrangianEmbedded::save( Serializer& rSerializer ) const
{
    KRATOS_SERIALIZE_SAVE_BASE_CLASS( rSerializer, Element )
    rSerializer.save("ConstitutiveLawVector",mConstitutiveLawVector);
    rSerializer.save("DeformationGradientF0",mDeformationGradientF0);
    rSerializer.save("DeterminantF0",mDeterminantF0);
    rSerializer.save("InverseJ0",mInverseJ0);
    rSerializer.save("DeterminantJ0",mDeterminantJ0);

}

void UpdatedLagrangianEmbedded::load( Serializer& rSerializer )
{
    KRATOS_SERIALIZE_LOAD_BASE_CLASS( rSerializer, Element )
    rSerializer.load("ConstitutiveLawVector",mConstitutiveLawVector);
    rSerializer.load("DeformationGradientF0",mDeformationGradientF0);
    rSerializer.load("DeterminantF0",mDeterminantF0);
    rSerializer.load("InverseJ0",mInverseJ0);
    rSerializer.load("DeterminantJ0",mDeterminantJ0);
}

} // Namespace Kratos

