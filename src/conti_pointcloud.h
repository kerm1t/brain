#ifndef __CONTIPOINT_H__
#define __CONTIPOINT_H__

#include <tuple>
#include <array>
#include <vector>
#include <cmath>
#include <memory>

namespace contipc
{
  class contipoint : public std::array<float, 3>
  {

  public:
    static const int DIM = 3;

    contipoint(float x, float y, float z)
    {
        (*this)[0] = x;
        (*this)[1] = y;
        (*this)[2] = z;
    }

    contipoint(std::vector<float> xyz) { 
        (*this)[0] = xyz[0];
        (*this)[1] = xyz[1];
        (*this)[2] = xyz[2];
    }

    float getValues(int indice) const
    {
        return (*this)[indice];
    }

    void setValues(int indice, float value)
    {
        (*this)[indice]= value;
    }
};

    // Structure contiPointCloud to store a collection of contipoints
    struct contiPointCloud
    {
      std::vector<contipoint> points {};

      contiPointCloud() {}
      contiPointCloud(const std::vector<contipoint>& points_vector) : points(points_vector) {}
    };

    struct contiPointWithScale {
      float x;
      float y;
      float z;
      float size;
      int   octave;

      std::vector<float> getxyz() { return std::vector<float> {x, y, z}; }
    };

    // Structure contindices to store a collection of indices from the pointcloud
    struct contindices
    {
      std::vector<int> indices {};

      contindices() {}
      contindices(const std::vector<int>& ind) : indices(ind) {}
    };

    
    using contiAABB = std::pair<contipoint, contipoint>; // Alias for a pair of contipoints

    // Struct to catalog matches between points and the distance between them
    struct contiCorrespondence
    {
      int index_query; // Index of source point
      int index_match = -1; // Index of matching point, if no match defaults to -1
      float distance; // distance between points

      contiCorrespondence(int query, int match, float dist) 
      { 
          index_query = query;
          index_match = match;
          distance    = dist;
      }
    };

    /**
     * Computes the size of the box surrounding the cluster
     *
     * @param contiAABB: pair of contipoints
     * 
     * @return tuple of bounding box measures (length, width, height)
     */
    inline std::tuple<float, float, float> getAABBSize(contiAABB aabb)
    {
      return std::make_tuple(std::abs(aabb.second[0] - aabb.first[0]),
                             std::abs(aabb.second[1] - aabb.first[1]),
                             std::abs(aabb.second[2] - aabb.first[2]));
    }

  /**
   * Extract the min and max in the 3 dimensions from a pointcloud
   *
   * @param contiPointCloud: Points from where to extract min and max
   *
   * @return contiAABB containing min and max
   */
  inline contiAABB getMinMax3D(const contiPointCloud &pc)
  {
    contipoint min(10000, 10000, 10000);
    contipoint max(-10000, -10000, -10000);
    for (unsigned int i{}; i < pc.points.size(); ++i)
    {
      for (unsigned int dim{}; dim < pc.points[i].size(); ++dim)
      {
        if (pc.points[i][dim] < min[dim])
        {
          min[dim] = pc.points[i][dim];
        }

        if (pc.points[i][dim] > max[dim])
        {
          max[dim] = pc.points[i][dim];
        }
      }
    }
    return contiAABB(min, max);
  }

  inline float deg2rad(float alpha) { return (alpha * 0.017453293f); }

  // calculate the center of the AABB
  inline contipoint getAABBcenter(const contiAABB cluster)
  {
	  float x_center = (cluster.first[0] + cluster.second[0]) / 2;
	  float y_center = (cluster.first[1] + cluster.second[1]) / 2;
	  float z_center = (cluster.first[2] + cluster.second[2]) / 2;

      contipoint center(x_center,y_center,z_center);

      return center;
  }
}

namespace contisearch
{
  class BruteForce
  {
  public:

    BruteForce(std::vector<contipc::contiPointWithScale>& points) : inputCloud(points) 
    { 
        cloudSize = points.size();
    }

    std::pair<unsigned int, float> NNSearch(const contipc::contiPointWithScale& query) 
    { 
        float dist {0.0f};
        float min_dist {100000.0f};
        int min_dist_idx {-1};
        for (unsigned int i{}; i < cloudSize; ++i)
        {
          dist = squaredDistance(inputCloud[i], query);
          if (dist < min_dist)
          {
            min_dist = dist;
            min_dist_idx = i;
          }
        }
        return std::make_pair(min_dist_idx, min_dist);
    }

    float squaredDistance(contipc::contiPointWithScale p1, contipc::contiPointWithScale p2)
    {
      return (p1.x - p2.x) * (p1.x - p2.x) +
             (p1.y - p2.y) * (p1.y - p2.y) +
             (p1.z - p2.z) * (p1.z - p2.z) +
             (p1.size - p2.size) * (p1.size - p2.size);
    }

   private:
    std::vector<contipc::contiPointWithScale>& inputCloud;
    unsigned int cloudSize;
  };
}

namespace contigeom
{
  inline float euclideanDistance(contipc::contipoint& p1, contipc::contipoint& p2)
  {
    float dist = 0;
    for (unsigned int i {}; i < p1.size(); ++i)
    {
    dist += (p1[i] - p2[i]) * (p1[i] - p2[i]);
    }
    return std::sqrt(dist);
  }

  inline std::vector<float> contipointDifference(const contipc::contipoint& p1, const contipc::contipoint& p2) 
  {
    return std::vector<float> {p1[0] - p2[0], p1[1] - p2[1], p1[2] - p2[2]};
  }
}

#endif
