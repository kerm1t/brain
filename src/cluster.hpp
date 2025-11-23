#ifndef GEN_CLUSTER_HPP
#define GEN_CLUSTER_HPP
/*
  static memory allocation
*/
#include <algorithm>
#include <cmath>
#include <chrono>
#include <vector>
#include <string>

#include "pointcloud/pointcloud.hpp"

#include "kdtree.h"
//#include "params.h"
#include "dbscan.hpp"

namespace /*gen::*/cluster
{
  /**
   * Create a clusters from indices
   *
   * @tparam PointT The point type
   * @return std::vector<std::shared_ptr<contipc::contiPointCloud>>
   */
/*  std::vector<std::shared_ptr<contipc::contiPointCloud>>
  createClusterFromIndices(std::shared_ptr<contipc::contiPointCloud> point_cloud,
                           const std::vector<contipc::contindices> &clusters_indices)
  {
    std::vector<std::shared_ptr<contipc::contiPointCloud>> clusters;
    for (const auto &cluster_indice : clusters_indices)
    {
      auto cloud_cluster{std::make_shared<contipc::contiPointCloud>()};
      for (auto indice : cluster_indice.indices)
      {
        cloud_cluster->points.push_back((*point_cloud).points[static_cast<unsigned int>(indice)]);
      }
      clusters.emplace_back(cloud_cluster);
    }

    return clusters;
  }
*/

  /**
   * Do the Clustering using the RBNN algorithm
   *
   * @tparam PointT type of the point cloud
   * @param point_cloud Complete point cloud
   * @param inliers Indices of non ground points of the point cloud
   * @return  std::pair<std::vector<std::shared_ptr<contipc::contiPointCloud>>, std::vector<contipc::contindices>>
   */
  std::pair<std::vector<int>, std::vector<int>> createRBNN_KNNClusters(const lloft::pointcloud& pcl)
  {
  /*
    - build kd-tree for all pts
    - for each point
      - find neighbors (with knn or rbnn)
      - over all neighbors of pt:
        - put into cluster with the pt
        - in case merge clusters
  */
    std::vector<int> pointClusterId(pcl.numpoints, -1); // for each pt: store the id of cluster

    // Create kd Tree
    kdt::KDTree<lloft::point> kdtree(pcl.pts_raw, pcl.numpoints);

    // Read params form json
    float radius = .2f;//param.rbnn_radius;
    float intensity_threshold = 100;//param.intensity_threshold;
    int min_cluster_points = 2;//param.min_cluster_points;
    // k - nearest neighbors.
    auto k = 4;//  param.knn;
    std::string clustering_method = "rbnn";//param.clustering_method;
    int currentCluster = 0;

//    std::vector<bool> visited;
//    visited.resize(pcl.numpoints, false);

    // loop through each point not belonging to ground or below ground
    for (int i=0; i < pcl.numpoints; i++)
    {
//      if (visited[i] == false)
      if (pointClusterId[i] == -1) // not yet visited (i.e. as neighbor)
      {
        // calculate beam diameter (beam divergance)
        float beam_angle = 0.0006981317; // 0.04 in grad
        float beam_diameter =  (2 * pcl.pts_raw[i].x * tan(beam_angle)) + 0.0001;
        
        std::vector<int> neighbors;

        // Fills the neighbor_indices vector with inliers indices of all neighbors to the current point (pointID)

        if (clustering_method == "knn")
        {
          neighbors = kdtree.knnSearch(pcl.pts_raw[i], k);
        }
        else if (clustering_method == "rbnn")
        {
          neighbors = kdtree.radiusSearch(pcl.pts_raw[i], radius+beam_diameter);
        }

        // This method is used to check if all points have a cluster and all neighbor points have a cluster.
        int pointID = i;
        for (int j=0; j < neighbors.size(); j++)
        {
          // Check if point + neighbor are in a cluster
          if ((pointClusterId[neighbors[j]] != -1) && (pointClusterId[pointID] != -1))
          {
            if (pointClusterId[neighbors[j]] != pointClusterId[pointID]) // if in different clusters ...
            {
              // ... Merge clusters
              auto firstClusterId = pointClusterId[neighbors[j]];
              auto secondClusterId = pointClusterId[pointID];
              const int pointClusterIdSize = pointClusterId.size();

              for (unsigned int k{0}; k < static_cast<unsigned int>(pointClusterIdSize); ++k)
              {
                if (pointClusterId[k] == firstClusterId)
                {
                  pointClusterId[k] = secondClusterId;
                }
              }
            }
          }
          else // check if there is a cluster in the neighbor or the current point
          {
            if (pointClusterId[neighbors[j]] != -1)
            {
              // Classify current point as the neighbor cluster
              pointClusterId[pointID] = pointClusterId[neighbors[j]];
            }

            else if (pointClusterId[pointID] != -1)
            {
              // Classify neighbor points as the current cluster
              pointClusterId[neighbors[j]] = pointClusterId[pointID];
            }
          }
        }

        if (pointClusterId[pointID] == -1)
        {
          pointClusterId[pointID] = currentCluster;

          for (unsigned int j{0}; j < static_cast<unsigned int>(neighbors.size()); ++j)
          {
            // Classify all neighbor points as the current cluster
            pointClusterId[neighbors[j]] = currentCluster;
          }
        }

        ++currentCluster; // Then we go to the next cluster
      }
    }

    // Save cluster indices in the format required for the output
    // We need to take the indices form the inliers
/*
    std::vector<int> clusters_indices;
    clusters_indices.resize(currentCluster + 1);

    const int pointClusterIdSize = pointClusterId.size();
    for (unsigned int i{0}; i < static_cast<unsigned int>(pointClusterIdSize); ++i)
    {
      if (pointClusterId[i] != -1)
      {
        clusters_indices[pointClusterId[i]].indices.push_back(inliers->indices[i]);
      }
    }

    // delete not filled clusters
    for (int i{currentCluster}; i >= 0; --i)
    {
      if (clusters_indices[i].indices.size() == 0)
      {
        clusters_indices.erase(clusters_indices.begin() + i);
      }
    }

    auto total_clusters = clusters_indices.size();

    clusters_mean_intensity.reserve(total_clusters);

    auto clusters{createClusterFromIndices(point_cloud, clusters_indices)};
    return std::make_pair(std::move(clusters), std::move(clusters_indices));
*/
    int ncluster = 0;
    std::vector<int> clusters_indices;
    clusters_indices.resize(currentCluster + 1, -1);
    for (int i=0; i < pcl.numpoints; i++)
    {
      int cid = pointClusterId[i];
      if (clusters_indices[cid] != -1)
      {
        clusters_indices[cid]=ncluster++;
      }
    }

    return {pointClusterId, clusters_indices}; // DUMMY, 2do: replace
  }
} // namespace gen::cluster

#endif // ifndef GEN_CLUSTER_HPP
