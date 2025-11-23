/*
  LT-Stack
  (C) 2025
  author: w.schulz
  
  eCAL Manager
  - create subscriber callbacks, publishers
  - message to internal structure?

  Ecal routines: Subscribe, publish
*/

#ifndef APP_ECAL_H
#define APP_ECAL_H

#include <string>

// eCAL
#include <ecal/ecal.h>
#include <ecal/msg/protobuf/publisher.h>
#include <ecal/msg/protobuf/subscriber.h>

// proto i/o
#include "pcl.pb.h"
#include "lane.pb.h"
#include "foxglove/LinePrimitive.pb.h"
#include "foxglove/PointCloud.pb.h"
#include "foxglove/SceneUpdate.pb.h"

// in
//#define FRS_PTS_TOPIC  "gse_pts"
#define FRS_PTS_TOPIC  "frs_out"
//#define FRS_PTS_TOPIC  "gse_out"
// out
#define OBJECTS_TOPIC "gen_obj" // wrapped in Foxglove Scene

namespace app_ecal {
// in
  eCAL::protobuf::CSubscriber<pcl::PointCloud2>      subscriber_frs_pts;
// out
  eCAL::protobuf::CPublisher<foxglove::SceneUpdate>  publisher_foxScene(OBJECTS_TOPIC);

  void Pointcloud_Callback(const pcl::PointCloud2 pointcloud_msg);

  void init(int argc, char** argv) {
    // Initialize eCAL
    eCAL::Initialize(argc, argv, ECAL_TITLE);
    // Create a protobuf subscriber
    subscriber_frs_pts = eCAL::protobuf::CSubscriber<pcl::PointCloud2>(FRS_PTS_TOPIC);
    // Set the Callback
    subscriber_frs_pts.AddReceiveCallback(std::bind(&Pointcloud_Callback, std::placeholders::_2));
    // create publisher
    publisher_foxScene = eCAL::protobuf::CPublisher<foxglove::SceneUpdate>(OBJECTS_TOPIC);
    // set eCAL state to healthy (--> eCAL Monitor)
//    eCAL::Process::SetState(proc_sev_healthy, proc_sev_level1, _strcat(COMP_NAME," eCAL publishers initialized"));
//    eCAL::Process::SetState(proc_sev_healthy, proc_sev_level1, "GEN eCAL publishers initialized");
//    char* status;
//    status = "GEN eCAL publishers initialized";
//    char* status1;
//    status1 = _strcat(COMP_NAME, " eCAL publishers initialized");
//    eCAL::Process::SetState(proc_sev_healthy, proc_sev_level1, status);
    eCAL::Process::SetState(proc_sev_healthy, proc_sev_level1, ECAL_INIT_OK);
  }

  void exit() {
    subscriber_frs_pts.Destroy();
    publisher_foxScene.Destroy();
    eCAL::Finalize();
  }

  uint64_t secs_nsecs_to_ts(uint32_t secs, uint32_t nsecs) {
    return (uint64_t)secs * 10 * 10 * 10 * 10 * 10 * 10 * 10 * 10 * 10 + nsecs;
  }
  void Pointcloud_Callback(const pcl::PointCloud2 pointcloud_msg)
  {
    const std::lock_guard<std::recursive_mutex> lock(m_SubscriberMutex);

//    pio.ecal_callback(pointcloud_msg); // 2do: directly give this to AddReceiveCallback
    pio.ecalmsg_2_pointcloud(pointcloud_msg);

    p.timestamp = secs_nsecs_to_ts(pointcloud_msg.header().stamp().secs(),pointcloud_msg.header().stamp().nsecs());
    p.secs = pointcloud_msg.header().stamp().secs();
    p.nsecs = pointcloud_msg.header().stamp().nsecs();

    memcpy(lloft::vertices, p.pts_raw.data(), p.numpoints * 3 * sizeof(float));
///2do    memcpy(lloft::colors, p.pts_rgb.data(), p.numpoints * 3 * sizeof(float));
///    p.pts_seg.resize(p.numpoints);
///    memcpy(lloft::colors, p.pts_seg.data(), p.numpoints * 3 * sizeof(float));
// <-- erst nach dem segmentieren

    b_newframe = true;
  }

  void publish(eCAL::protobuf::CPublisher<pcl::PointCloud2>& publisher_frs) {
    // Create a protobuf message object
    pcl::PointCloud2 frs_pointcloud;
    std::vector<float> pts;
    for (int i = 0; i < p.pts_raw.size(); i++)
    {
      pts.push_back(p.pts_raw[i].x); pts.push_back(p.pts_raw[i].y); pts.push_back(p.pts_raw[i].z);
      pts.push_back(p.pts_rgb[i].r); pts.push_back(p.pts_rgb[i].g); pts.push_back(p.pts_rgb[i].b);
      pts.push_back(p.pts_class[i]);
    }
    // Create a protobuf message object
    setPointCloud(&frs_pointcloud, { "x","y","z","Processed Intensity","g","b","class" }, pts);// optimize: have individual field types
    frs_pointcloud.mutable_header()->mutable_stamp()->set_secs(p.secs);
    frs_pointcloud.mutable_header()->mutable_stamp()->set_nsecs(p.nsecs);
    // Send the message
    publisher_frs.Send(frs_pointcloud);
  }

  void publish_fox_pointcloud(eCAL::protobuf::CPublisher<foxglove::PointCloud>& publisher_gse_fox) {
    foxglove::PointCloud pcl_fox;

    // 2do: even easier: just hand the pointcloud and the serializer will mem-copy itself
    std::vector<float> pts;
    for (int i = 0; i < p.pts_raw.size(); i++)
    {
      pts.push_back(p.pts_raw[i].x); pts.push_back(p.pts_raw[i].y); pts.push_back(p.pts_raw[i].z);
      pts.push_back(p.pts_rgb[i].r); pts.push_back(p.pts_rgb[i].g); pts.push_back(p.pts_rgb[i].b);
      pts.push_back(p.pts_class[i]);
    }
///2do: unify     set_fox_PointCloud(&pcl_fox, { "x","y","z","Processed Intensity","g","b","class" }, pts);// optimize: have individual field types

    publisher_gse_fox.Send(pcl_fox);
  }

  void publish_fox_poly(eCAL::protobuf::CPublisher<foxglove::SceneUpdate>& publisher_foxScene) {
    foxglove::SceneUpdate scu;
    foxglove::SceneEntity* sce;
    foxglove::LinePrimitive* poly;

    sce = scu.mutable_entities()->Add();
    poly = sce->mutable_lines()->Add();
    for (int i = 0; i < p_freespace->nPoly; i++) {
      poly->mutable_points()->Add();
      int max_idx = poly->mutable_points()->size() - 1;
      poly->mutable_points(max_idx)->set_x(p_freespace->a_Poly[i].x);
      poly->mutable_points(max_idx)->set_y(p_freespace->a_Poly[i].y);
      // set colors in Foxglove / Lichtblick
    }
    poly->set_thickness(.3f);
    poly->set_type(foxglove::LinePrimitive_Type_LINE_STRIP);
    sce->set_frame_id("base_link");
    publisher_foxScene.Send(scu);
  }

}

#endif// APP_ECAL_H
