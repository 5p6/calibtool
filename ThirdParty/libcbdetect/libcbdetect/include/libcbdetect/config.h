
#pragma once
#ifndef LIBCBDETECT_CONFIG_H
#define LIBCBDETECT_CONFIG_H

#include <opencv2/opencv.hpp>
#include <vector>

#ifdef _MSC_VER
#define M_PI 3.14159265358979323846   /* pi */
#define M_PI_2 1.57079632679489661923 /* pi/2 */
#define M_PI_4 0.78539816339744830962 /* pi/4 */
#define M_1_PI 0.31830988618379067154 /* 1/pi */
#define M_2_PI 0.63661977236758134308 /* 2/pi */
#endif

#ifndef LIBCBDETECT_DLL_DECL
#if IS_A_DLL && defined(_MSC_VER)
#define LIBCBDETECT_DLL_DECL __declspec(dllexport)
#else
#define LIBCBDETECT_DLL_DECL
#endif
#endif

namespace cbdetect {

	class specialCompare
	{
	public:

		bool operator () (const cv::Point2i &a, const cv::Point2i &b) const
		{
			if (a == b)
			{
				return false;
			}
			else
			{
				return (static_cast<float>(a.x)*tan(M_PI / 12) + static_cast<float>(a.y)*sin(M_PI / 2)) < (static_cast<float>(b.x)*tan(M_PI / 12) + static_cast<float>(b.y)*sin(M_PI / 2));
			}
		}
	};


enum DetectMethod {
  // origin method fast mode
  TemplateMatchFast = 0,
  // origin method slow mode
  TemplateMatchSlow,

  // compute hessian of image, detect by a threshold
  // form https://github.com/facebookincubator/deltille
  HessianResponse,

  // paper: Accurate Detection and Localization of Checkerboard Corners for Calibration
  LocalizedRadonTransform
};

enum CornerType {
  SaddlePoint = 0,
  MonkeySaddlePoint
};

	enum CameraPosition
	{
		
		FRONT=0,
		RIGHT,		
		BACK,
		LEFT
	};
		
	enum BoardPosition
	{
		RightBoard = 0,
		MiddleBoard,
		LeftBoard
	};

	enum class BoardOriginType
	{
		LeftUpSame=0,
		LeftUpDiff,
		RightUpSame,
		RightUpDiff,
		LeftDownSame,
		LeftDownDiff,
		RightDownSame,
		RightDownDiff,
	};

	using chessboardObject = std::map<cbdetect::CameraPosition, std::map<cbdetect::BoardPosition, std::map<cv::Point2i, cv::Point2d, specialCompare>>>;
typedef struct Params {
  bool show_processing;
  bool show_debug_image;
  bool show_grow_processing;
  bool norm;
  bool polynomial_fit;
  int norm_half_kernel_size;
  int polynomial_fit_half_kernel_size;
  double init_loc_thr;
  double score_thr;
  bool strict_grow;
  bool occlusion;
  DetectMethod detect_method;
  CornerType corner_type;
  std::vector<int> radius;

  Params()
      : show_processing(true)
      , show_debug_image(false)//false
      , show_grow_processing(false)
      , norm(false)
      , polynomial_fit(true)
      , norm_half_kernel_size(31)
      , polynomial_fit_half_kernel_size(4)
      , init_loc_thr(0.01)
      , score_thr(0.01)
      , strict_grow(true)
      , occlusion(true)
      , detect_method(HessianResponse)
      , corner_type(SaddlePoint)
      , radius({5, 7}) {}
} Params;

typedef struct Corner {
  std::vector<cv::Point2d> p;
  std::vector<int> r;
  std::vector<cv::Point2d> v1;
  std::vector<cv::Point2d> v2;
  std::vector<cv::Point2d> v3;
  std::vector<double> score;
} Corner;

typedef struct Board {
  std::vector<std::vector<int>> idx;
  std::vector<std::vector<cv::Point2i>> objectIdx;
  std::vector<std::vector<cv::Point2i>> objectInverseIdx;
  std::vector<std::vector<std::vector<double>>> energy;
  int num;
  BoardPosition position;
  cv::Point2d center;

  Board()
      : num(0) {}
} Board;

} // namespace cbdetect

#endif //LIBCBDETECT_CONFIG_H
