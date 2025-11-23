#ifndef __KDTREE_H__
#define __KDTREE_H__

#include <vector>
#include <numeric>
#include <algorithm>
#include <exception>
#include <functional>
//#include "conti_pointcloud.h"
#include "pointcloud/pointcloud.hpp"

namespace kdt
{
  /** @brief k-d tree class.
   */
  template <class PointT>
  class KDTree
  {
  public:
    // "hack" replace point access by [i] through axxess via .x/.y/.z
//  float pt_axis(const PointT& pt, const int axis) {         // error
    float pt_axis(const PointT& pt, const int axis) const {   // correct
      if (axis == 0) return pt.x;
      if (axis == 1) return pt.y;
      if (axis == 2) return pt.z;
    }

    /** @brief The constructors.
    */
    KDTree() : root_(nullptr) {};
    KDTree(const std::vector<PointT>& points, unsigned int dimension) : root_(nullptr), pt_dim(dimension) { build(points); }

    /** @brief The destructor.
     */
    ~KDTree() { clear(); }

    /** @brief Re-builds k-d tree.
     */
    void build(const std::vector<PointT>& points)
    {
      clear();

      points_s = points;

      std::vector<int> indices(points.size());
      std::iota(std::begin(indices), std::end(indices), 0);

      root_ = buildRecursive(indices.data(), (int)points.size(), 0);
    }

    /** @brief Clears k-d tree.
     */
    void clear()
    {
      clearRecursive(root_);
      root_ = nullptr;
      points_s.clear();
    }

    /** @brief Validates k-d tree.
     */
    bool validate() const
    {
      try
      {
        validateRecursive(root_, 0);
      }
      catch (const Exception&)
      {
        return false;
      }

      return true;
    }

    /** @brief Searches the nearest neighbor.
     */
    int nnSearch(const PointT& query, double* minDist = nullptr) const
    {
      int guess;
      double _minDist = std::numeric_limits<double>::max();

      nnSearchRecursive(query, root_, &guess, &_minDist);

      if (minDist)
        *minDist = _minDist;

      return guess;
    }

    /** @brief Searches k-nearest neighbors.
     */
    std::vector<int> knnSearch(const PointT& query, int k) const
    {
      KnnQueue queue(k);
      knnSearchRecursive(query, root_, queue, k);

      std::vector<int> indices(queue.size());
      for (size_t i = 0; i < queue.size(); i++)
        indices[i] = queue[i].second;

      return indices;
    }

    /** @brief Searches neighbors within radius.
     */
    std::vector<int> radiusSearch(const PointT& query, float radius) const
    {
      std::vector<int> indices;
      radiusSearchRecursive(query, root_, indices, radius);
      return indices;
    }

  private:
    /** @brief k-d tree node.
     */
    struct Node
    {
      int idx;	   //!< index to the original point
      Node* next[2]; //!< pointers to the child nodes
      int axis;	   //!< dimension's axis

      Node() : idx(-1), axis(-1) { next[0] = next[1] = nullptr; }
    };

    /** @brief k-d tree exception.
     */
    class Exception : public std::exception
    {
      using std::exception::exception;
    };

    /** @brief Bounded priority queue.
     */
    template <class T, class Compare = std::less<T>>
    class BoundedPriorityQueue
    {
    public:
      BoundedPriorityQueue() = delete;
      BoundedPriorityQueue(size_t bound) : bound_(bound) { elements_.reserve(bound + 1); };

      void push(const T& val)
      {
        auto it = std::find_if(std::begin(elements_), std::end(elements_),
          [&](const T& element)
          { return Compare()(val, element); });
        elements_.insert(it, val);

        if (elements_.size() > bound_)
          elements_.resize(bound_);
      }

      const T& back() const { return elements_.back(); };
      const T& operator[](size_t index) const { return elements_[index]; }
      size_t size() const { return elements_.size(); }

    private:
      size_t bound_;
      std::vector<T> elements_;
    };

    /** @brief Priority queue of <distance, index> pair.
     */
    using KnnQueue = BoundedPriorityQueue<std::pair<double, int>>;

    /** @brief Builds k-d tree recursively.
     */
    Node* buildRecursive(int* indices, int npoints, int depth)
    {
      if (npoints <= 0)
        return nullptr;

      const int axis = depth % pt_dim;
      const int mid = (npoints - 1) / 2;

      std::nth_element(indices, indices + mid, indices + npoints, [&](int lhs, int rhs)
        { return pt_axis(points_s[lhs],axis) < pt_axis(points_s[rhs],axis); });

      Node* node = new Node();
      node->idx = indices[mid];
      node->axis = axis;

      node->next[0] = buildRecursive(indices, mid, depth + 1);
      node->next[1] = buildRecursive(indices + mid + 1, npoints - mid - 1, depth + 1);

      return node;
    }

    /** @brief Clears k-d tree recursively.
     */
    void clearRecursive(Node* node)
    {
      if (node == nullptr)
        return;

      if (node->next[0])
        clearRecursive(node->next[0]);

      if (node->next[1])
        clearRecursive(node->next[1]);

      delete node;
    }

    /** @brief Validates k-d tree recursively.
     */
    void validateRecursive(const Node* node, int depth) const
    {
      if (node == nullptr)
        return;

      const int axis = node->axis;
      const Node* node0 = node->next[0];
      const Node* node1 = node->next[1];

      if (node0 && node1)
      {
        if (points_s[node->idx][axis] < points_s[node0->idx][axis])
          throw Exception();

        if (points_s[node->idx][axis] > points_s[node1->idx][axis])
          throw Exception();
      }

      if (node0)
        validateRecursive(node0, depth + 1);

      if (node1)
        validateRecursive(node1, depth + 1);
    }

    static double distance(const PointT& p, const PointT& q)
    {
      double dist = (p.x - q.x) * (p.x - q.x);
//      for (size_t i = 0; i < PointT::DIM; i++)
//        dist += (p[i] - q[i]) * (p[i] - q[i]);
      dist += (p.y - q.y) * (p.y - q.y);
      dist += (p.z - q.z) * (p.z - q.z);
      return sqrt(dist);
    }

    /** @brief Searches the nearest neighbor recursively.
     */
    void nnSearchRecursive(const PointT& query, const Node* node, int* guess, double* minDist) const
    {
      if (node == nullptr)
        return;

      const PointT& train = points_s[node->idx];

      const double dist = distance(query, train);
      if (dist < *minDist)
      {
        *minDist = dist;
        *guess = node->idx;
      }

      const int axis = node->axis;
      const int dir = query[axis] < train[axis] ? 0 : 1;
      nnSearchRecursive(query, node->next[dir], guess, minDist);

      const double diff = fabs(query[axis] - train[axis]);
      if (diff < *minDist)
        nnSearchRecursive(query, node->next[!dir], guess, minDist);
    }

    /** @brief Searches k-nearest neighbors recursively.
     */
    void knnSearchRecursive(const PointT& query, const Node* node, KnnQueue& queue, int k) const
    {
      if (node == nullptr)
        return;

      const PointT& train = points_s[node->idx];

      const double dist = distance(query, train);
      queue.push(std::make_pair(dist, node->idx));

      const int axis = node->axis;
      const int dir = pt_axis(query,axis) < pt_axis(train,axis) ? 0 : 1;
      knnSearchRecursive(query, node->next[dir], queue, k);

      const double diff = fabs(pt_axis(query,axis) - pt_axis(train,axis));
      if ((int)queue.size() < k || diff < queue.back().first)
        knnSearchRecursive(query, node->next[!dir], queue, k);
    }

    /** @brief Searches neighbors within radius.
     */
    void radiusSearchRecursive(const PointT& query, const Node* node, std::vector<int>& indices, float radius) const
    {
      if (node == nullptr)
        return;
      const PointT& train = points_s[node->idx];

      const double dist = distance(query, train);
      float adaptive_radius = radius + (query.x * ((0.145 * 3.1416) / 180)); // deg2rad
      if (dist <= adaptive_radius)
      {
        indices.push_back(node->idx);
      }

      const int axis = node->axis;
      const int dir = pt_axis(query,axis) < pt_axis(train,axis) ? 0 : 1;
      radiusSearchRecursive(query, node->next[dir], indices, radius);

      const double diff = fabs(pt_axis(query,axis) - pt_axis(train,axis));
      if (diff < radius)
        radiusSearchRecursive(query, node->next[!dir], indices, radius);
    }

    Node* root_;                  //!< root node
    std::vector<PointT> points_s; //!< points
    unsigned int  pt_dim;         //!< point dimension
  };
} // kdt

#endif // !__KDTREE_H__
