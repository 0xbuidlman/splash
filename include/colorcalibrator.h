/*
 * Copyright (C) 2015 Emmanuel Durand
 *
 * This file is part of Splash.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * blobserver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with blobserver.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * @colorcalibrator.h
 * The ColorCalibrator class
 */

#ifndef SPLASH_COLORCALIBRATOR_H
#define SPLASH_COLORCALIBRATOR_H

#include "config.h"
#include "coretypes.h"
#include "basetypes.h"

#include <utility>

namespace pic {
class Image;
class CameraResponseFunction;
}

namespace Splash {

class World;

/*************/
class ColorCalibrator : public BaseObject
{
    public:
        /**
         * Constructor
         */
        ColorCalibrator(std::weak_ptr<World> world);

        /**
         * Destructor
         */
        ~ColorCalibrator();

        /**
         * No copy constructor
         */
        ColorCalibrator(const ColorCalibrator&) = delete;
        ColorCalibrator& operator=(const ColorCalibrator&) = delete;

        ColorCalibrator(ColorCalibrator&& c)
        {
            *this = std::move(c);
        }

        ColorCalibrator& operator=(ColorCalibrator&& c)
        {
            if (this != &c)
            {
            }
            return *this;
        }

        /**
         * Update the color calibration of all cameras
         */
        void update();

        /**
         * Update the color response function of the physical camera
         */
        void updateCRF();

    private:
        // Some internal types
        typedef std::pair<float, float> Point;
        typedef std::vector<Point> Curve;

        struct CalibrationParams
        {
            std::vector<int> camPos {2};
            std::vector<float> minValues {3};
            std::vector<float> maxValues {3};
            std::vector<Curve> curves {3};
            std::vector<Curve> projectorCurves;
        };

        // Attributes
        std::weak_ptr<World> _world;
        Image_GPhotoPtr _gcamera;
        std::shared_ptr<pic::CameraResponseFunction> _crf {nullptr};

        unsigned int _nbrImageHDR {3}; // Number of images to use for HDR capture
        double _hdrStep {1.0}; // Number of stops between to LDR images
        unsigned int _colorCurveSamples {5}; // Number of samples for each channels to create the color curves
        int _meanBoxSize {32}; // Size of the box over which to compute the mean value

        /**
         * Capture an HDR image from the gcamera
         */
        std::shared_ptr<pic::Image> captureHDR();

        /**
         * Compute the inverse projection transformation function, typically
         * correcting the projector non linearity for all three channels
         */
        std::vector<Curve> computeProjectorFunctionInverse(std::vector<Curve> rgbCurves);

        /**
         * Find the exposure which gives correctly exposed photos
         */
        void findCorrectExposure();

        /**
         * Find the center of region with max values
         */
        std::vector<int> getMaxRegionCenter(std::shared_ptr<pic::Image> image);

        /**
         * Get the mean value of the area around the given coords
         */
        std::vector<float> getMeanValue(std::shared_ptr<pic::Image> image, std::vector<int> coords = std::vector<int>());

        /**
         * Register new functors to modify attributes
         */
        void registerAttributes();
};

typedef std::shared_ptr<ColorCalibrator> ColorCalibratorPtr;

} // end of namespace

#endif