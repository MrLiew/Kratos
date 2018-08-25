//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		BSD License
//					Kratos default license: kratos/license.txt
//
//  Main authors:    Bodhinanda Chandra
//
#if !defined(KRATOS_MC_STRAIN_SOFTENING_PLASTIC_FLOW_RULE_H_INCLUDED )
#define      KRATOS_MC_STRAIN_SOFTENING_PLASTIC_FLOW_RULE_H_INCLUDED


// System includes

// External includes

#include<cmath>
// Project includes
#include "custom_constitutive/flow_rules/mc_plastic_flow_rule.hpp"


namespace Kratos
{
///@addtogroup ApplicationNameApplication
///@{

///@name Kratos Globals
///@{

///@}
///@name Type Definitions
///@{

//struct MCStressInvariants {

//double MeanStress;
//double J2InvSQ;
//double LodeAngle;

//};

//struct MCSmoothingConstants {

//double A;
//double B;

//};
///@}
///@name  Enum's
///@{

///@}
///@name  Functions
///@{

///@}
///@name Kratos Classes
///@{

/// Short class definition.
/** Detail class definition.
 */
class MCStrainSofteningPlasticFlowRule
    :public MCPlasticFlowRule
{



public:
    ///@name Type Definitions
    ///@{

    /// Pointer definition of NonLinearAssociativePlasticFlowRule
    KRATOS_CLASS_POINTER_DEFINITION( MCStrainSofteningPlasticFlowRule );

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.
    MCStrainSofteningPlasticFlowRule();

    /// Initialization constructor.
    MCStrainSofteningPlasticFlowRule(YieldCriterionPointer pYieldCriterion);

    /// Copy constructor.
    MCStrainSofteningPlasticFlowRule(MCStrainSofteningPlasticFlowRule const& rOther);

    /// Assignment operator.
    MCStrainSofteningPlasticFlowRule& operator=(MCStrainSofteningPlasticFlowRule const& rOther);

    // CLONE
    MPMFlowRule::Pointer Clone() const override;

    /// Destructor.
    ~MCStrainSofteningPlasticFlowRule() override;

    // bool CalculateReturnMapping( RadialReturnVariables& rReturnMappingVariables, const Matrix& rIncrementalDeformationGradient, Matrix& rStressMatrix, Matrix& rNewElasticLeftCauchyGreen) override;

    // bool UpdateInternalVariables( RadialReturnVariables& rReturnMappingVariables ) override;

    // Matrix GetElasticLeftCauchyGreen(RadialReturnVariables& rReturnMappingVariables) override;

    // void ComputeElastoPlasticTangentMatrix(const RadialReturnVariables& rReturnMappingVariables, const Matrix& rNewElasticLeftCauchyGreen, const double& alfa, Matrix& rConsistMatrix) override;
    ///@}
    ///@name Operators
    ///@{


    ///@}
    ///@name Operations
    ///@{


    ///@}
    ///@name Access
    ///@{


    ///@}
    ///@name Inquiry
    ///@{


    ///@}
    ///@name Input and output
    ///@{

    ///@}
    ///@name Friends
    ///@{


    ///@}
   
    //virtual void GetPrincipalStressAndStrain(Vector& PrincipalStresses, Vector& PrincipalStrains);
    ///@}
    ///@name Protected  Access
    ///@{


    ///@}
    ///@name Protected Inquiry
    ///@{


    ///@}
    ///@name Protected LifeCycle
    ///@{


    ///@}

private:
    ///@name Static Member Variables
    ///@{


    ///@}
    ///@name Member Variables
    ///@{


    ///@}
    ///@name Private Operators
    ///@{


    ///@}
    ///@name Private Operations
    ///@{


    ///@}
    ///@name Private  Access
    ///@{


    ///@}
    ///@name Serialization
    ///@{
    friend class Serializer;

    // A private default constructor necessary for serialization

    void save(Serializer& rSerializer) const override;

    void load(Serializer& rSerializer) override;

    ///@}
    ///@name Private Inquiry
    ///@{


    ///@}
    ///@name Un accessible methods
    ///@{

    ///@}

}; // Class NonLinearAssociativePlasticFlowRule

///@}

///@name Type Definitions
///@{


///@}
///@name Input and output
///@{


/// input stream function
// inline std::istream& operator >> (std::istream& rIStream,
// 				    NonLinearAssociativePlasticFlowRule& rThis);

// /// output stream function
// inline std::ostream& operator << (std::ostream& rOStream,
// 				    const NonLinearAssociativePlasticFlowRule& rThis)
// {
//   rThis.PrintInfo(rOStream);
//   rOStream << std::endl;
//   rThis.PrintData(rOStream);

//   return rOStream;
// }
///@}

///@} addtogroup block



///@}
///@ Template Operations
///@{


///@}


}  // namespace Kratos.

#endif // KRATOS_MC_STRAIN_SOFTENING_PLASTIC_FLOW_RULE_H_INCLUDED  defined 
