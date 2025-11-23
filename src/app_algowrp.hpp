/*
  libLT Algo Wrapper
  (C) 2025
  author: w.schulz

  < description of this files >

  - C++ style guide: https://google.github.io/styleguide/cppguide.html
    [-] NOT YET ACCOMPLISHED
  - Test with CPPLINT: https://github.com/google/styleguide/tree/gh-pages/cpplint
    [-] NOT YET DONE
*/

#ifndef APP_ALGOWRP_H
#define APP_ALGOWRP_H

#include <chrono>
#include <iostream> // 2do: replace cout with proper logging

#include "LT_app.h"

#include "pointcloud/pointcloud.hpp"
#include "cluster.hpp"
#include "util/color.h"

namespace algo {

  void run_cluster_pts();

  bool b_computed;// = true;


  // Algo : Ground segmentation
  void run_cluster_pts() {
    b_computed = false;

    auto t1 = std::chrono::high_resolution_clock::now();

    /*
      ------------------------------------------------
      run algo here
      - loop over points as they come from FRS
                                                - non-ground
                                                - within road 
          = gen::cluster::ClusterContainerNew{ point_cloud, inliers, gsmPoint, intensity }
        - choose clustering algo, e.g. RBNN, dbScan, ...
          = createRBNN_KNNClusters(global_point_cloud, inliers, intensity, cluster_mean_intensity)
        - filterClusters(gsmPoint)
      - loop over bboxes
        - expandBboxtoGround(*global_point_cloud) // optional
        - geom::computeAABB(cluster_object_pair.first)
        - combine BBoxes ? ? ?
      - append bboxes to objectlist
      ------------------------------------------------
    */
//    std::vector<int> clu = cluster::createRBNN_KNNClusters(p);
    std::pair<std::vector<int>, std::vector<int>> pair;
    pair = cluster::createRBNN_KNNClusters(p);

    // 2do: do this directly in createRBNN...
    // distinct color / cluster, s. https://stackoverflow.com/questions/2328339/how-to-generate-n-different-colors-for-any-natural-number-n
    p.pts_seg.resize(p.numpoints);
    for (int i = 0; i < p.numpoints; i++) {
//      if (i < 127)
//      int j = i % 128;
      int j = pair.second[pair.first[i]];// % 128;
      {
        p.pts_seg[i].r = (colors128[j] >> 16) & 0xff;
        p.pts_seg[i].g = (colors128[j] >> 8)  & 0xff;
        p.pts_seg[i].b =  colors128[j]        & 0xff;
      }
    }
    memcpy(lloft::colors, p.pts_seg.data(), p.numpoints * 3 * sizeof(float));
    
/*    auto clu = dbscan(p, 0.7f, 5);
    p.pts_seg.resize(p.numpoints);
    for (int i = 0; i < clu.size(); i++) {
      for (int j = 0; j < clu[i].size(); j++) {
        int iPt = clu[i][j];
        int iCol = i;
//        if (iCol > 127) iCol = 127;
        p.pts_seg[iPt].r = (colors128[iCol] >> 16) & 0xff;
        p.pts_seg[iPt].g = (colors128[iCol] >> 8)  & 0xff;
        p.pts_seg[iPt].b =  colors128[iCol]        & 0xff;
      }
    }
    memcpy(lloft::colors, p.pts_seg.data(), p.numpoints * 3 * sizeof(float));
    */
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = t2 - t1;
    std::cout << "algo<1> ground " << ms_double.count() << "ms\n";

    //    b_computed = true; // 2do: set this after frs and g_model
  }

} // namespace algo

#endif// APP_ALGOWRP_H
