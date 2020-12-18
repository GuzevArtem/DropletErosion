#pragma once

#include <glm/vec3.hpp>
#include "Grid.hpp"

class NormalMapGenerator
{
public:
    template<typename grid_cell_type>
    static inline Grid<glm::f64vec3> caclulateWorldSpaceNormalFromHeightMap(const Grid<grid_cell_type>& heightMap,
                                                                            const double pixel_to_meter_ratio_x = 1,
                                                                            const double pixel_to_meter_ratio_y = 1,
                                                                            const size_t channel = 0)
    {
        Grid<glm::f64vec3> result(heightMap.get_x_size(), heightMap.get_y_size());

        heightMap.for_each_par ([channel, &result, &heightMap, pixel_to_meter_ratio_x, pixel_to_meter_ratio_y](const auto x, const auto y, const grid_cell_type value) -> void
                           {
                               const bool Xis0 = x == 0;
                               const bool Yis0 = y == 0;
                               const bool XisMax = x == (heightMap.get_x_size() - 1);
                               const bool YisMax = y == (heightMap.get_y_size() - 1);

                               //to prevent warning only
                               const glm::length_t c = (glm::length_t)channel;

                               double nx = value[c];
                               double ny = value[c];
                               double px = value[c];
                               double py = value[c];
                               double nxpy = value[c];
                               double nxny = value[c];
                               double pxny = value[c];
                               double pxpy = value[c];
                               if (!Xis0)
                               {
                                   nx = heightMap.at_unchecked(x - 1, y)[c];
                               }
                               if (!XisMax)
                               {
                                   px = heightMap.at_unchecked(x + 1, y)[c];
                               }
                               if (!Yis0)
                               {
                                   ny = heightMap.at_unchecked(x, y - 1)[c];
                               }
                               if (!YisMax)
                               {
                                   py = heightMap.at_unchecked(x, y + 1)[c];
                               }
                               if (!Xis0 && !Yis0)
                               {
                                   nxny = heightMap.at_unchecked(x - 1, y - 1)[c];
                               }
                               if (!Xis0 && !YisMax)
                               {
                                   nxpy = heightMap.at_unchecked(x - 1, y + 1)[c];
                               }
                               if (!XisMax && !Yis0)
                               {
                                   pxny = heightMap.at_unchecked(x + 1, y - 1)[c];
                               }
                               if (!XisMax && !YisMax)
                               {
                                   pxpy = heightMap.at_unchecked(x + 1, y + 1)[c];
                               }

                               //interpolated values
                               const double inter_nx = (nxny + nxpy + 3 * nx) / 5;
                               const double inter_px = (pxny + pxpy + 3 * px) / 5;
                               const double inter_ny = (nxny + pxny + 3 * ny) / 5;
                               const double inter_py = (nxpy + pxpy + 3 * py) / 5;

                               const glm::f64vec3 v1{ 2.0 / pixel_to_meter_ratio_x, 0.0, (inter_px - inter_nx) };
                               const glm::f64vec3 v2{ 0.0, 2.0 / pixel_to_meter_ratio_y, (inter_py - inter_ny) };

                               const glm::f64vec3 n = glm::cross (v1, v2);

                               result.assign_unchecked(x ,y, glm::normalize(n));
                           });

        return result;
    }

    //aka tangent space normal
    template<typename grid_cell_type>
    static inline Grid<glm::f64vec3> caclulateNormalFromHeightMap(const Grid<grid_cell_type>& heightMap,
                                                                  const double pixel_to_meter_ratio_x = 1,
                                                                  const double pixel_to_meter_ratio_y = 1, 
                                                                  const size_t channel = 0)
    {
        Grid<glm::f64vec3> result(heightMap.get_x_size(), heightMap.get_y_size());

        heightMap.for_each_par ([channel, &result, &heightMap, pixel_to_meter_ratio_x, pixel_to_meter_ratio_y](const auto x, const auto y, const grid_cell_type value) -> void
                           {
                               const bool Xis0 = x == 0;
                               const bool Yis0 = y == 0;
                               const bool XisMax = x == (heightMap.get_x_size() - 1);
                               const bool YisMax = y == (heightMap.get_y_size() - 1);

                               glm::f64vec3 p(0, 0, value[channel]);
                               glm::f64vec3 nx(-1, 0, value[channel]);
                               glm::f64vec3 ny(0, -1, value[channel]);
                               glm::f64vec3 px(1, 0, value[channel]);
                               glm::f64vec3 py(0, 1, value[channel]);
                               glm::f64vec3 nxpy(-1, 1, value[channel]);
                               glm::f64vec3 nxny(-1, -1, value[channel]);
                               glm::f64vec3 pxny(1, -1, value[channel]);
                               glm::f64vec3 pxpy(1, 1, value[channel]);
                               if (!Xis0)
                               {
                                   nx[2] = heightMap.at_unchecked(x - 1, y)[channel];
                               }
                               if (!XisMax)
                               {
                                   px[2] = heightMap.at_unchecked(x + 1, y)[channel];
                               }
                               if (!Yis0)
                               {
                                   ny[2] = heightMap.at_unchecked(x, y - 1)[channel];
                               }
                               if (!YisMax)
                               {
                                   py[2] = heightMap.at_unchecked(x, y + 1)[channel];
                               }
                               if (!Xis0 && !Yis0)
                               {
                                   nxny[2] = heightMap.at_unchecked(x - 1, y - 1)[channel];
                               }
                               if (!Xis0 && !YisMax)
                               {
                                   nxpy[2] = heightMap.at_unchecked(x - 1, y + 1)[channel];
                               }
                               if (!XisMax && !Yis0)
                               {
                                   pxny[2] = heightMap.at_unchecked(x + 1, y - 1)[channel];
                               }
                               if (!XisMax && !YisMax)
                               {
                                   pxpy[2] = heightMap.at_unchecked(x + 1, y + 1)[channel];
                               }

                               glm::f64vec3 nnxny{};
                               glm::f64vec3 nnxpy{};
                               glm::f64vec3 npxny{};
                               glm::f64vec3 npxpy{};

                               {// p, nx, ny, nxny
                                   const glm::f64vec3 v1 = p - nxny;
                                   const glm::f64vec3 v2 = nx - ny;

                                   const glm::f64vec3 n = glm::cross(v1, v2);
                                   nnxny = glm::normalize(n);
                               }
                               {// p, nx, py, nxpy
                                   const glm::f64vec3 v1 = nxpy - p;
                                   const glm::f64vec3 v2 = py - ny;

                                   const glm::f64vec3 n = glm::cross(v1, v2);
                                   nnxpy = glm::normalize(n);
                               }
                               {// p, px, ny, pxny
                                   const glm::f64vec3 v1 = p - pxny;
                                   const glm::f64vec3 v2 = px - ny;

                                   const glm::f64vec3 n = glm::cross(v1, v2);
                                   npxny = glm::normalize(n);
                               }
                               {// p, px, py, pxpy
                                   const glm::f64vec3 v1 = pxpy - p;
                                   const glm::f64vec3 v2 = py - px;

                                   const glm::f64vec3 n = glm::cross(v1, v2);
                                   npxpy = glm::normalize(n);
                               }

                               const double normal_x = nnxny[0] + nnxpy[0] + npxny[0] + npxpy[0];
                               const double normal_y = nnxny[1] + nnxpy[1] + npxny[1] + npxpy[1];
                               const double normal_z = nnxny[2] + nnxpy[2] + npxny[2] + npxpy[2];

                               const double normal_x_normalized = (normal_x) / (2 * pixel_to_meter_ratio_x);
                               const double normal_y_normalized = (normal_y) / (2 * pixel_to_meter_ratio_y);
                               const double normal_z_normalized = -1;

                               const glm::f64vec3 normal(
                                   normal_x_normalized,
                                   normal_y_normalized,
                                   normal_z_normalized
                               );
                               result.assign_unchecked(x, y, glm::normalize(normal));
                           });
        return result;
    }

    //specialization

    template<>
    static inline Grid<glm::f64vec3> caclulateWorldSpaceNormalFromHeightMap <double> (const Grid<double>& heightMap,
                                                                             const double pixel_to_meter_ratio_x,
                                                                             const double pixel_to_meter_ratio_y,
                                                                             const size_t channel)
    {
        Grid<glm::f64vec3> result (heightMap.get_x_size (), heightMap.get_y_size ());

        heightMap.for_each_par ([channel, &result, &heightMap, pixel_to_meter_ratio_x, pixel_to_meter_ratio_y](const auto x, const auto y, const double value) -> void
                            {
                                const bool Xis0 = x == 0;
                                const bool Yis0 = y == 0;
                                const bool XisMax = x == (heightMap.get_x_size () - 1);
                                const bool YisMax = y == (heightMap.get_y_size () - 1);

                                double nx = value;
                                double ny = value;
                                double px = value;
                                double py = value;
                                double nxpy = value;
                                double nxny = value;
                                double pxny = value;
                                double pxpy = value;
                                if ( !Xis0 )
                                {
                                    nx = heightMap.at_unchecked (x - 1, y);
                                }
                                if ( !XisMax )
                                {
                                    px = heightMap.at_unchecked (x + 1, y);
                                }
                                if ( !Yis0 )
                                {
                                    ny = heightMap.at_unchecked (x, y - 1);
                                }
                                if ( !YisMax )
                                {
                                    py = heightMap.at_unchecked (x, y + 1);
                                }
                                if ( !Xis0 && !Yis0 )
                                {
                                    nxny = heightMap.at_unchecked (x - 1, y - 1);
                                }
                                if ( !Xis0 && !YisMax )
                                {
                                    nxpy = heightMap.at_unchecked (x - 1, y + 1);
                                }
                                if ( !XisMax && !Yis0 )
                                {
                                    pxny = heightMap.at_unchecked (x + 1, y - 1);
                                }
                                if ( !XisMax && !YisMax )
                                {
                                    pxpy = heightMap.at_unchecked (x + 1, y + 1);
                                }

                                //interpolated values
                                const double inter_nx = (nxny + nxpy + 3 * nx) / 5;
                                const double inter_px = (pxny + pxpy + 3 * px) / 5;
                                const double inter_ny = (nxny + pxny + 3 * ny) / 5;
                                const double inter_py = (nxpy + pxpy + 3 * py) / 5;

                                const glm::f64vec3 v1{ 2.0 / pixel_to_meter_ratio_x, 0.0, (inter_px - inter_nx) };
                                const glm::f64vec3 v2{ 0.0, 2.0 / pixel_to_meter_ratio_y, (inter_py - inter_ny) };

                                const glm::f64vec3 n = glm::cross (v1, v2);

                                result.assign_unchecked (x, y, glm::normalize (n));
                            });

        return result;
    }

    //aka tangent space normal
    template<>
    static inline Grid<glm::f64vec3> caclulateNormalFromHeightMap <double> (const Grid<double>& heightMap,
                                                                   const double pixel_to_meter_ratio_x,
                                                                   const double pixel_to_meter_ratio_y,
                                                                   const size_t channel)
    {
        Grid<glm::f64vec3> result (heightMap.get_x_size (), heightMap.get_y_size ());

        heightMap.for_each_par ([channel, &result, &heightMap, pixel_to_meter_ratio_x, pixel_to_meter_ratio_y](const auto x, const auto y, const double value) -> void
                            {
                                const bool Xis0 = x == 0;
                                const bool Yis0 = y == 0;
                                const bool XisMax = x == (heightMap.get_x_size () - 1);
                                const bool YisMax = y == (heightMap.get_y_size () - 1);

                                glm::f64vec3 p (0, 0, value);
                                glm::f64vec3 nx (-1, 0, value);
                                glm::f64vec3 ny (0, -1, value);
                                glm::f64vec3 px (1, 0, value);
                                glm::f64vec3 py (0, 1, value);
                                glm::f64vec3 nxpy (-1, 1, value);
                                glm::f64vec3 nxny (-1, -1, value);
                                glm::f64vec3 pxny (1, -1, value);
                                glm::f64vec3 pxpy (1, 1, value);
                                if ( !Xis0 )
                                {
                                    nx[2] = heightMap.at_unchecked (x - 1, y);
                                }
                                if ( !XisMax )
                                {
                                    px[2] = heightMap.at_unchecked (x + 1, y);
                                }
                                if ( !Yis0 )
                                {
                                    ny[2] = heightMap.at_unchecked (x, y - 1);
                                }
                                if ( !YisMax )
                                {
                                    py[2] = heightMap.at_unchecked (x, y + 1);
                                }
                                if ( !Xis0 && !Yis0 )
                                {
                                    nxny[2] = heightMap.at_unchecked (x - 1, y - 1);
                                }
                                if ( !Xis0 && !YisMax )
                                {
                                    nxpy[2] = heightMap.at_unchecked (x - 1, y + 1);
                                }
                                if ( !XisMax && !Yis0 )
                                {
                                    pxny[2] = heightMap.at_unchecked (x + 1, y - 1);
                                }
                                if ( !XisMax && !YisMax )
                                {
                                    pxpy[2] = heightMap.at_unchecked (x + 1, y + 1);
                                }

                                glm::f64vec3 nnxny{};
                                glm::f64vec3 nnxpy{};
                                glm::f64vec3 npxny{};
                                glm::f64vec3 npxpy{};

                                {// p, nx, ny, nxny
                                    const glm::f64vec3 v1 = p - nxny;
                                    const glm::f64vec3 v2 = nx - ny;

                                    const glm::f64vec3 n = glm::cross (v1, v2);
                                    nnxny = glm::normalize (n);
                                }
                                {// p, nx, py, nxpy
                                    const glm::f64vec3 v1 = nxpy - p;
                                    const glm::f64vec3 v2 = py - ny;

                                    const glm::f64vec3 n = glm::cross (v1, v2);
                                    nnxpy = glm::normalize (n);
                                }
                                {// p, px, ny, pxny
                                    const glm::f64vec3 v1 = p - pxny;
                                    const glm::f64vec3 v2 = px - ny;

                                    const glm::f64vec3 n = glm::cross (v1, v2);
                                    npxny = glm::normalize (n);
                                }
                                {// p, px, py, pxpy
                                    const glm::f64vec3 v1 = pxpy - p;
                                    const glm::f64vec3 v2 = py - px;

                                    const glm::f64vec3 n = glm::cross (v1, v2);
                                    npxpy = glm::normalize (n);
                                }

                                const double normal_x = nnxny[0] + nnxpy[0] + npxny[0] + npxpy[0];
                                const double normal_y = nnxny[1] + nnxpy[1] + npxny[1] + npxpy[1];
                                const double normal_z = nnxny[2] + nnxpy[2] + npxny[2] + npxpy[2];

                                const double normal_x_normalized = (normal_x) / (2 * pixel_to_meter_ratio_x);
                                const double normal_y_normalized = (normal_y) / (2 * pixel_to_meter_ratio_y);
                                const double normal_z_normalized = -1;

                                const glm::f64vec3 normal (
                                    normal_x_normalized,
                                    normal_y_normalized,
                                    normal_z_normalized
                                );
                                result.assign_unchecked (x, y, glm::normalize (normal));
                            });
        return result;
    }
};
