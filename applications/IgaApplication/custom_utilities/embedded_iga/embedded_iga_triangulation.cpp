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
#include "embedded_iga_triangulation.h"

namespace Kratos
{
void EmbeddedIgaTriangulation::CreateTriangulation(
    const std::vector<std::vector<array_1d<double,2>>>& rOuterPolygon,
    const std::vector<std::vector<array_1d<double,2>>>& rInnerPolygon,
    std::vector<Matrix>& rTriangulation)
{
    /**
     * This function generates a triangulation of the patch in the parametric space
    */

    // initializing the i/o containers
    struct triangulateio in_data; 
    struct triangulateio out_data; 
    struct triangulateio vor_out_data;

    InitTriangulationDataStructure(in_data); 
    InitTriangulationDataStructure(out_data); 
    InitTriangulationDataStructure(vor_out_data); 

    // Initialize the pointlist (1d list) with the number of points and the coordinates
    // of the points (outer and inner polygons) 
    unsigned int number_points = 0; 
    for (unsigned int i = 0; i < rOuterPolygon.size(); ++i)
    {
        number_points += rOuterPolygon[i].size(); 
    }
    for (unsigned int i = 0; i < rInnerPolygon.size(); ++i)
    {
        number_points += rInnerPolygon[i].size(); 
    }
    
    in_data.numberofpoints = number_points; 
    in_data.pointlist = (REAL*) malloc(in_data.numberofpoints * 2 * sizeof(REAL));
    in_data.pointmarkerlist = (int*) malloc(in_data.numberofpoints * sizeof(int));

    unsigned int point_index = 0;
    unsigned int point_marker_index = 0;
    unsigned int point_marker = 0; 
    for (unsigned int poly_i = 0; poly_i < rOuterPolygon.size(); ++poly_i)
    {
        for (unsigned int node_i = 0; node_i < rOuterPolygon[poly_i].size(); ++node_i)    
        {
            for (unsigned int coords_i = 0; coords_i < 2; ++coords_i)
            {
                in_data.pointlist[point_index++] = rOuterPolygon[poly_i][node_i][coords_i];
            }
            in_data.pointmarkerlist[point_marker_index++] = point_marker; 
        }
        point_marker++;
    }

    for (unsigned int poly_i = 0; poly_i < rInnerPolygon.size(); ++poly_i)
    {
        for (unsigned int node_i = 0; node_i < rInnerPolygon[poly_i].size(); ++node_i)
        {
            for (unsigned int coords_i = 0; coords_i < 2; ++coords_i)    
            {
                in_data.pointlist[point_index++] = rInnerPolygon[poly_i][node_i][coords_i]; 
            }
            in_data.pointmarkerlist[point_marker_index++] = point_marker; 
        }
        point_marker++;
    }

    // Initilize the segment list with the number of boundary edges and the start and end node id
    // For closed polygons the number of segments is equal to the number of points
    in_data.numberofsegments = number_points; 
    in_data.segmentlist = (int*) malloc(in_data.numberofsegments * 2 * sizeof(int));
    in_data.segmentmarkerlist = (int*) malloc(in_data.numberofsegments * sizeof(int));
    
    unsigned int seg_index = 0; 
    unsigned int node_id = 0; 
    unsigned int seg_marker = 0; 
    unsigned int start_node_id = 0;
    unsigned int end_node_id = 0;
     
    for (unsigned int poly_i = 0; poly_i < rOuterPolygon.size(); ++poly_i)
    {
        end_node_id += rOuterPolygon[poly_i].size(); 

        for (unsigned int seg_i = start_node_id * 2 ; seg_i < end_node_id * 2; ++seg_i)
        {

            in_data.segmentlist[seg_i] = node_id;

            if (node_id == end_node_id)    
            {
                in_data.segmentlist[seg_i] = start_node_id; 
            }
            if (seg_i % 2 == 0)   
            {
                in_data.segmentmarkerlist[seg_i/2] = seg_marker; 
                node_id++; 
            }
        }
        seg_marker++;
        start_node_id = end_node_id;
    }

    for (unsigned int poly_i = 0; poly_i < rInnerPolygon.size(); ++poly_i)
    {
        end_node_id += rInnerPolygon[poly_i].size(); 

        for (unsigned int seg_i = start_node_id * 2 ; seg_i < end_node_id * 2; ++seg_i)
        {

            in_data.segmentlist[seg_i] = node_id;

            if (node_id == end_node_id)    
            {
                in_data.segmentlist[seg_i] = start_node_id; 
            }
            if (seg_i % 2 == 0)   
            {
                in_data.segmentmarkerlist[seg_i/2] = seg_marker; 
                node_id++; 
            }
        }
        seg_marker++;
        start_node_id = end_node_id;
    }

    // in_data.numberofholes = rInnerPolygon.size(); 
    // in_data.holelist = (REAL*) malloc(in_data.numberofholes * 2 * sizeof(REAL));

    // in_data.holelist[0] = 3; 
    // in_data.holelist[1] = 2.5;
    // in_data.holelist[2] = 6; 
    // in_data.holelist[3] = 3; 

    
    char trigenOptsVerbose[] = "Dpza1"; 
    char* trigenOpts = trigenOptsVerbose; 

    triangulate(trigenOpts, &in_data, &out_data, &vor_out_data);

    rTriangulation.resize(out_data.numberoftriangles, ZeroMatrix(3,2)); 

    unsigned int tri_id = 0; 
    for (unsigned int i = 0; i < out_data.numberoftriangles; ++i)
    {
        for (unsigned int j = 0; j < 3; ++j)
        {   
            rTriangulation[i](j,0) = out_data.pointlist[out_data.trianglelist[tri_id + j] * 2];
            rTriangulation[i](j,1) = out_data.pointlist[out_data.trianglelist[tri_id + j] * 2 + 1]; 
        }
        tri_id += 3;  
    }


    CleanTriangulationDataStructure(in_data); 
    CleanTriangulationDataStructure(out_data); 
    CleanTriangulationDataStructure(vor_out_data); 
   
}

EmbeddedIgaTriangulation::EmbeddedIgaTriangulation()
{}

} // namespace Kratos.