// KRATOS  ___|  |                   |                   |
//       \___ \  __|  __| |   |  __| __| |   |  __| _` | |
//             | |   |    |   | (    |   |   | |   (   | |
//       _____/ \__|_|   \__,_|\___|\__|\__,_|_|  \__,_|_| MECHANICS
//
//  License:		 BSD License
//					 license: structural_mechanics_application/license.txt
//
//  Main authors:   Alejandro Cornejo & Lucia Barbu
//

#if !defined(KRATOS_TANGENT_OPERATOR_CALCULATOR_UTILITY_H_INCLUDED )
#define  KRATOS_TANGENT_OPERATOR_CALCULATOR_UTILITY_H_INCLUDED

// System includes

// External includes

// Project includes
#include "processes/process.h"
#include "includes/constitutive_law.h"
#include <vector>

namespace Kratos
{

///@name Kratos Globals
///@{

///@}
///@name Type Definitions
///@{

///@}
///@name  Enum's
///@{

///@}
///@name  Functions
///@{

///@}
///@name Kratos Classes
///@{

/**
 * @class TangentOperatorCalculatorUtility
 * @ingroup StructuralMechanicsApplication
 * @brief An algorithm that derives numerically the constitutive tangent tensor at one GP
 * @details The procedure is defined in the PAPER "Caracterización de la delaminación en materiales
 compuestos mediante la teoría de mezclas serie/paralelo" X. Martinez, S. Oller y E. Barbero.
 * @authors Alejandro Cornejo & Lucia Barbu
 */
class KRATOS_API(STRUCTURAL_MECHANICS_APPLICATION) TangentOperatorCalculatorUtility
{
public:

    /// Pointer definition of TangentOperatorCalculatorUtility
    KRATOS_CLASS_POINTER_DEFINITION(TangentOperatorCalculatorUtility);

    /// Constructor
    TangentOperatorCalculatorUtility()
    {
    }

    /// Destructor.
    virtual ~TangentOperatorCalculatorUtility() {}

    static void CalculateTangentTensor(
        ConstitutiveLaw::Parameters& rValues,
        ConstitutiveLaw* pConstitutiveLaw
    )
    {
        // Converged values to be storaged
        const Vector StrainVectorGP = rValues.GetStrainVector();
        const Vector StressVectorGP = rValues.GetStressVector();

        Matrix& TangentTensor = rValues.GetConstitutiveMatrix();
        TangentTensor.clear();

        const int NumComp = StrainVectorGP.size();
        // Loop over components of the strain
        for (int Component = 0; Component < NumComp; Component++) {
            Vector& PerturbedStrain = rValues.GetStrainVector();
			
            double Perturbation;
            CalculatePerturbation(PerturbedStrain, Component, Perturbation);
            PerturbateStrainVector(PerturbedStrain, StrainVectorGP, Perturbation, Component);
            IntegratePerturbedStrain(rValues, pConstitutiveLaw);

            Vector& PerturbedIntegratedStress = rValues.GetStressVector(); // now integrated
            const Vector& DeltaStress = PerturbedIntegratedStress - StressVectorGP; 
            AssignComponentsToTangentTensor(TangentTensor, DeltaStress, Perturbation, Component);

            // Reset the values to the initial ones
            noalias(PerturbedStrain) = StrainVectorGP;
            noalias(PerturbedIntegratedStress) = StressVectorGP;
        }
    }

    static void CalculatePerturbation(
        const Vector& StrainVector, 
        const int Component,
        double& rPerturbation
    )
    {
        double Pert1, Pert2;
        if (StrainVector[Component] != 0.0) {
            Pert1 = 1.0e-5 * StrainVector[Component];
        } else {
            double MinStrainComp;
            GetMinAbsValue(StrainVector, MinStrainComp);
            Pert1 = 1.0e-5 * MinStrainComp;
        }
        double MaxStrainComp;
        GetMaxAbsValue(StrainVector, MaxStrainComp);
        Pert2 = 1e-10*MaxStrainComp;
        rPerturbation = std::max(Pert1, Pert2);
    }

    static void PerturbateStrainVector(
        Vector& PerturbedStrainVector, 
        const Vector& StrainVectorGP,
        const double Perturbation,
        const int Component
    )
    {
        PerturbedStrainVector = StrainVectorGP;
        PerturbedStrainVector[Component] += Perturbation;
    }

    static void IntegratePerturbedStrain(
        ConstitutiveLaw::Parameters& rValues,
        ConstitutiveLaw* pConstitutiveLaw
    )
    {
		Flags& ConstitutiveLawOptions = rValues.GetOptions();
        // In order to avoid recursivity...
        ConstitutiveLawOptions.Set(ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR, false);

        pConstitutiveLaw->CalculateMaterialResponseCauchy(rValues);
    }

    static void GetMaxAbsValue(
        const Vector& ArrayValues, 
        double& MaxValue
    )
    {
		const int Dim = ArrayValues.size();
		std::vector<double> non_zero_values;

		for (int i = 1; i < Dim; i++) {
			if (ArrayValues[i] != 0.0) non_zero_values.push_back(std::abs(ArrayValues[i]));
		}
		KRATOS_ERROR_IF(non_zero_values.size() == 0) << "The strain vector is full of 0's..." << std::endl;

		double aux = std::abs(non_zero_values[0]);
		for (int i = 1; i < non_zero_values.size(); i++) {
			if (non_zero_values[i] > aux) aux = non_zero_values[i];
		}

		MaxValue = aux;
    }

    static void GetMinAbsValue(
        const Vector& ArrayValues, 
        double& MinValue
    )
    {
        const int Dim = ArrayValues.size();
		std::vector<double> non_zero_values;

        for (int i = 0; i < Dim; i++) {
			if (ArrayValues[i] != 0.0) non_zero_values.push_back(std::abs(ArrayValues[i]));
        }
        KRATOS_ERROR_IF(non_zero_values.size() == 0) << "The strain vector is full of 0's..." << std::endl;

		double aux = std::abs(non_zero_values[0]);
		for (int i = 1; i < non_zero_values.size(); i++) {
			if (non_zero_values[i] < aux) aux = non_zero_values[i];
		}

		MinValue = aux;
    }

    static void AssignComponentsToTangentTensor(
        Matrix& TangentTensor, 
        const Vector& DeltaStress,
        const double Perturbation,
        const int Component
    )
    {
        const int Dim = DeltaStress.size();
        for (int row = 0; row < Dim; row++) {
            TangentTensor(row, Component) = DeltaStress[row] / Perturbation;
        }
    }

protected:
    ///@name Protected static Member Variables
    ///@{

    ///@}
    ///@name Protected member Variables
    ///@{

    ///@}
    ///@name Protected Operators
    ///@{

    ///@}
    ///@name Protected Operations
    ///@{

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
    ///@name Private Inquiry
    ///@{

    ///@}
    ///@name Un accessible methods
    ///@{

    /// Assignment operator.
    TangentOperatorCalculatorUtility& operator=(TangentOperatorCalculatorUtility const& rOther);

};
}// namespace Kratos.
#endif // KRATOS_TANGENT_OPERATOR_CALCULATOR_PROCESS_H_INCLUDED  defined