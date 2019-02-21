//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:     BSD License
//           Kratos default license: kratos/IGAStructuralMechanicsApplication/license.txt
//
//  Main authors:    
//

// System includes

// External includes

// Project includes
#include "embedded_iga_error_estimation.h"

namespace Kratos
{   
void EmbeddedIgaErrorEstimation::InsertGaussPointsExactSurface(
    const std::vector<Matrix>& rTriangulation_uv,
    std::vector<Matrix>& rGaussPoints_uv)
{
    /**
     * This function inserts Gauss-Legendre Points into the triangulation of the surface in the parametric domain.
    */

    const auto gp_canonical_tri = 
        Quadrature<TriangleGaussLegendreIntegrationPoints2, 2, IntegrationPoint<3> >::GenerateIntegrationPoints();


    rGaussPoints_uv.resize(rTriangulation_uv.size(), ZeroMatrix(gp_canonical_tri.size(),2)); 

    for (unsigned int tri_i = 0; tri_i < rTriangulation_uv.size(); ++tri_i)
    {    
        for (unsigned int gp_i = 0; gp_i < gp_canonical_tri.size(); ++gp_i)
        {
            rGaussPoints_uv[tri_i](gp_i,0) = rTriangulation_uv[tri_i](0,0) * (1 - gp_canonical_tri[gp_i][0] - gp_canonical_tri[gp_i][1]) + 
                                             rTriangulation_uv[tri_i](1,0) * gp_canonical_tri[gp_i][0] + 
                                             rTriangulation_uv[tri_i](2,0) * gp_canonical_tri[gp_i][1];

            rGaussPoints_uv[tri_i](gp_i,1) = rTriangulation_uv[tri_i](0,1) * (1 - gp_canonical_tri[gp_i][0] - gp_canonical_tri[gp_i][1]) + 
                                             rTriangulation_uv[tri_i](1,1) * gp_canonical_tri[gp_i][0] + 
                                             rTriangulation_uv[tri_i](2,1) * gp_canonical_tri[gp_i][1]; 
        }
    }
}

void EmbeddedIgaErrorEstimation::InsertGaussPointsApproxSurface(
    const std::vector<Matrix>& rTriangulation_xyz,
    std::vector<Matrix>& rGaussPoints_xyz)
{
    /**
     * This function inserts Gauss-Legendre points into the triangles approximating the exact Surface.
     * These points can be used to measure the distance between the approximated and exact surface
    */

    const auto gp_canonical_tri = 
        Quadrature<TriangleGaussLegendreIntegrationPoints2, 2, IntegrationPoint<3> >::GenerateIntegrationPoints();

    rGaussPoints_xyz.resize(rTriangulation_xyz.size(), ZeroMatrix(gp_canonical_tri.size(),3));  

    for (unsigned int tri_i = 0; tri_i < rTriangulation_xyz.size(); ++tri_i)
    {    
        for (unsigned int gp_i = 0; gp_i < gp_canonical_tri.size(); ++gp_i)
        {
            rGaussPoints_xyz[tri_i](gp_i,0) = rTriangulation_xyz[tri_i](0,0) * (1 - gp_canonical_tri[gp_i][0] - gp_canonical_tri[gp_i][1]) + 
                                              rTriangulation_xyz[tri_i](1,0) * gp_canonical_tri[gp_i][0] + 
                                              rTriangulation_xyz[tri_i](2,0) * gp_canonical_tri[gp_i][1];

            rGaussPoints_xyz[tri_i](gp_i,1) = rTriangulation_xyz[tri_i](0,1) * (1 - gp_canonical_tri[gp_i][0] - gp_canonical_tri[gp_i][1]) + 
                                              rTriangulation_xyz[tri_i](1,1) * gp_canonical_tri[gp_i][0] + 
                                              rTriangulation_xyz[tri_i](2,1) * gp_canonical_tri[gp_i][1]; 
        
            rGaussPoints_xyz[tri_i](gp_i,2) = rTriangulation_xyz[tri_i](0,2) * (1 - gp_canonical_tri[gp_i][0] - gp_canonical_tri[gp_i][1]) + 
                                              rTriangulation_xyz[tri_i](1,2) * gp_canonical_tri[gp_i][0] + 
                                              rTriangulation_xyz[tri_i](2,2) * gp_canonical_tri[gp_i][1]; 
        }
    }
}




    EmbeddedIgaErrorEstimation::EmbeddedIgaErrorEstimation()
    {}

} // namespace Kratos.
