/*
 * TemplateAlignment.h
 *
 *  Created on: Oct 24, 2012
 */

#ifndef TEMPLATEALIGNMENT_H_
#define TEMPLATEALIGNMENT_H_

#include <limits>
#include <fstream>
#include <vector>
#include <Eigen/Core>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/io/pcd_io.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/filters/passthrough.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/features/normal_3d.h>
#include <pcl/features/fpfh.h>
#include <pcl/registration/ia_ransac.h>
#include <ros/ros.h>
#include <tf/tfMessage.h>
#include <tf/LinearMath/Transform.h>



class TemplateAlignment
{

	typedef pcl::PointCloud<pcl::PointXYZ> PtCloud;
	typedef pcl::PointCloud<pcl::Normal> NormalsCloud;
	typedef pcl::PointCloud<pcl::FPFHSignature33> LocalFeatures;
	typedef pcl::search::KdTree<pcl::PointXYZ> SearchMethod;

public:
	struct ModelFeatureData
	{
	public:

		ModelFeatureData()
		:SearchMethod_(new SearchMethod()),
		 NormalRadius_(0.02f),
		 FeatureRadius_(0.02f),
		 PointCloud_(),
		 Normals_(),
		 Features_()
		{

		}

		ModelFeatureData(PtCloud::Ptr cloudPtr)
		:SearchMethod_(new SearchMethod()),
		 NormalRadius_(0.02f),
		 FeatureRadius_(0.02f),
		 PointCloud_(cloudPtr),
		 Normals_(),
		 Features_()
		{
			processData();
		}

		void setInputCloud(PtCloud::Ptr cloudPtr)
		{
			PointCloud_ = cloudPtr;
			processData();
		}

		bool loadInputCloud(std::string fileName)
		{
			PointCloud_ = boost::make_shared<PtCloud>();
			pcl::io::loadPCDFile<pcl::PointXYZ>(fileName,*PointCloud_);
			processData();
		}

		// data
		PtCloud::Ptr PointCloud_;
		NormalsCloud::Ptr Normals_;
		LocalFeatures::Ptr Features_;
		SearchMethod::Ptr SearchMethod_;

		// parameters
		double NormalRadius_;
		double FeatureRadius_;

	protected:

		void processData()
		{
			computeNormals();
			computeFeatures();
		}

		void computeNormals();
		void computeFeatures();

	};

	struct AlignmentResult
	{
		float FitnessScore_;
		Eigen::Matrix4f Transform_;
		int Index_;

	};


public:
	TemplateAlignment();
	virtual ~TemplateAlignment();

	void setCandidateModel(ModelFeatureData &candidate);
	void addModelTemplate(ModelFeatureData &templateData);
	const std::vector<ModelFeatureData>& getModelTemplates();
	bool findBestAlignment(AlignmentResult &result);

protected:

	void align(const ModelFeatureData &templateData, AlignmentResult &result);
	void align(const std::vector<ModelFeatureData> &templates,std::vector<AlignmentResult> &results);

	std::vector<ModelFeatureData> model_templates_;
	ModelFeatureData model_candidate_;

	pcl::SampleConsensusInitialAlignment<pcl::PointXYZ, pcl::PointXYZ, pcl::FPFHSignature33> sac_initial_alignment_;
	float min_sample_distance_;
	float max_correspondance_distance_;
	int max_iterations_;

};

#endif /* TEMPLATEALIGNMENT_H_ */
