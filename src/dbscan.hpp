#pragma once


#include <cassert>
#include <cstddef>
#include <span>
#include <vector>
#include <cstdlib>

#include <nanoflann.hpp>
#include <pointcloud/pointcloud.hpp>

struct point2
{
    float x, y;
};

struct point3
{
    float x, y, z;
};

//auto dbscan(const std::span<const point2>& data, float eps, int min_pts) -> std::vector<std::vector<size_t>>;
//auto dbscan(const std::span<const point3>& data, float eps, int min_pts) -> std::vector<std::vector<size_t>>;

// template<size_t dim>
// auto dbscan(const std::span<float>& data, float eps, int min_pts)
// {
//     static_assert(dim == 2 or dim == 3, "This only supports either 2D or 3D points");
//     assert(data.size() % dim == 0);
    
//     if(dim == 2)
//     {
//         auto * const ptr = reinterpret_cast<float const*> (data.data());
//         auto points = std::span<const point2> 
//     }
// }

template <typename T>
struct PointCloud
{
  struct Point
  {
    T x, y, z;
  };

  using coord_t = T;  //!< The type of each coordinate

  std::vector<Point> pts;

  // Must return the number of data points
  inline size_t kdtree_get_point_count() const { return pts.size(); }

  // Returns the dim'th component of the idx'th point in the class:
  // Since this is inlined and the "dim" argument is typically an immediate
  // value, the
  //  "if/else's" are actually solved at compile time.
  inline T kdtree_get_pt(const size_t idx, const size_t dim) const
  {
    if (dim == 0)
      return pts[idx].x;
    else if (dim == 1)
      return pts[idx].y;
    else
      return pts[idx].z;
  }

  // Optional bounding-box computation: return false to default to a standard
  // bbox computation loop.
  //   Return true if the BBOX was already computed by the class and returned
  //   in "bb" so it can be avoided to redo it again. Look at bb.size() to
  //   find out the expected dimensionality (e.g. 2 or 3 for point clouds)
  template <class BBOX>
  bool kdtree_get_bbox(BBOX& /* bb */) const
  {
    return false;
  }
};

auto sort_clusters(std::vector<std::vector<size_t>>& clusters)
{
  for (auto& cluster : clusters)
  {
    std::sort(cluster.begin(), cluster.end());
  }
}

auto dbscan(const lloft::pointcloud& p, float eps, int min_pts)
{
  PointCloud<float> cloud;
  cloud.pts.resize(p.numpoints);
  for (size_t i = 0; i < static_cast<size_t>(p.numpoints); i++)
  {
    cloud.pts[i].x = p.pts_raw[i].x;
    cloud.pts[i].y = p.pts_raw[i].y;
    cloud.pts[i].z = p.pts_raw[i].z;
  }

  eps *= eps;
//  using namespace nanoflann;
  using my_kd_tree_t = nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<float, PointCloud<float>>, PointCloud<float>, 3>;

  my_kd_tree_t index(3, cloud, {10});
  index.buildIndex();

//  const auto n_points = adapt.kdtree_get_point_count();
  int n_points = p.numpoints;
  auto visited = std::vector<bool>(n_points);
  auto clusters = std::vector<std::vector<size_t>>();
  auto matches = std::vector<std::pair<size_t, float>>();
  auto sub_matches = std::vector<std::pair<size_t, float>>();

  for (size_t i = 0; i < n_points; i++)
  {
    if (visited[i]) continue;

    index.radiusSearch(&cloud.pts[i].x, eps, matches, nanoflann::SearchParams(32, 0.f, false));
    if (matches.size() < static_cast<size_t>(min_pts)) continue;
    visited[i] = true;

//    std::vector<int> cluster;// = std::vector({ i });
    auto cluster = std::vector<size_t>();

    while (matches.empty() == false)
    {
      auto nb_idx = matches.back().first;
      matches.pop_back();
      if (visited[nb_idx]) continue;
      visited[nb_idx] = true;

      index.radiusSearch(&cloud.pts[nb_idx].x, eps, sub_matches, nanoflann::SearchParams(32, 0.f, false));

      if (sub_matches.size() >= static_cast<size_t>(min_pts))
      {
        std::copy(sub_matches.begin(), sub_matches.end(), std::back_inserter(matches));
      }
      cluster.push_back(nb_idx);
    }
    clusters.emplace_back(std::move(cluster));
  }

  sort_clusters(clusters);
  return clusters;
}
