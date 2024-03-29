#include "../hdr/ColorSystemTypes.hpp"
#include <functional>

using std::vector;

static const double EPS = 1e-10;

double f(double t)
{
    return (t >= 0.008856 + EPS) ? std::pow(t, 1. / 3.) : 7.787 * t + 16. / 116.;
}

double gamma_correction(double t)
{
    return (t <= 0.04045) ? t / 12.92 : std::pow((t + 0.055) / 1.055, 2.4);
}

RGB::operator XYZ () const
{
    vector<vector<Point<double>>> newValuesResource;
    newValuesResource.reserve(resource.size());

    std::for_each(resource.begin(), resource.end(), [&](const std::vector<Point<u_char>> row) {
        vector<Point<double>> newRow; newRow.reserve(row.size());

        std::for_each(row.begin(), row.end(), [&](Point<u_char> point) {

            std::tuple<u_char, u_char, u_char> point_tuple = static_cast<std::tuple<u_char, u_char, u_char>>(point);

            double r = static_cast<double>(std::get<0>(point_tuple));
            double g = static_cast<double>(std::get<1>(point_tuple));
            double b = static_cast<double>(std::get<2>(point_tuple));

            r = gamma_correction(r / 255.);
            g = gamma_correction(g / 255.);
            b = gamma_correction(b / 255.);

            double x = 0.412453 * r + 0.357780 * g + 0.180423 * b;
            double y = 0.212671 * r + 0.715160 * g + 0.072169 * b;
            double z = 0.019334 * r + 0.119193 * g + 0.950227 * b;

            newRow.emplace_back(x, y, z);
            });
        newValuesResource.emplace_back(newRow);
        });
    return XYZ(newValuesResource);
}

RGB::operator HSV () const
{
    vector<vector<Point<double>>> newValuesResource;
    newValuesResource.reserve(resource.size());

    std::for_each(resource.begin(), resource.end(), [&](const std::vector<Point<u_char>> row) {
        vector<Point<double>> newRow; newRow.reserve(row.size());

        std::for_each(row.begin(), row.end(), [&](Point<u_char> point) {

            std::tuple<u_char, u_char, u_char> point_tuple = static_cast<std::tuple<u_char, u_char, u_char>>(point);

            double r = static_cast<double>(std::get<0>(point_tuple)); r /= 255.;
            double g = static_cast<double>(std::get<1>(point_tuple)); g /= 255.;
            double b = static_cast<double>(std::get<2>(point_tuple)); b /= 255.;

            double v = std::max(r, std::max(g, b));

            double c = std::min(r, std::min(g, b));
            double s = (std::abs(v - EPS) > 0.) && v > EPS ? (v - c) / v : 0.;

            double h = -1.;

            if (std::abs(g - r) < EPS && std::abs(g - b) < EPS)
                h = 0.;
            else
            {
                if (std::abs(v - r) < EPS)
                {
                    h = 60. * (g - b) / (v - c);
                }
                else if (std::abs(v - g) < EPS)
                {
                    h = 120. + 60. * (b - r) / (v - c);
                }
                else if (std::abs(v - b) < EPS)
                {
                    h = 240. + 60. * (r - g) / (v - c);
                }
            }
            if (h < 0.) h += 360.;
            newRow.emplace_back(h / 2., s * 255., v * 255.);
            });
        newValuesResource.emplace_back(newRow);
        });
    return HSV(newValuesResource);
}

RGB::operator HLS() const
{
    vector<vector<Point<double>>> newValuesResource;
    newValuesResource.reserve(resource.size());

    std::for_each(resource.begin(), resource.end(), [&](const std::vector<Point<u_char>> row) {
        vector<Point<double>> newRow; newRow.reserve(row.size());

        std::for_each(row.begin(), row.end(), [&](Point<u_char> point) {

            std::tuple<u_char, u_char, u_char> point_tuple = static_cast<std::tuple<u_char, u_char, u_char>>(point);

            double r = static_cast<double>(std::get<0>(point_tuple)); r /= 255.;
            double g = static_cast<double>(std::get<1>(point_tuple)); g /= 255.;
            double b = static_cast<double>(std::get<2>(point_tuple)); b /= 255.;

            double v_max = std::max(r, std::max(g, b));
            double v_min = std::min(r, std::min(g, b));
            double l = (v_max + v_min) / 2.;

            double s = (l <= 0.5 + EPS && std::abs(v_max + v_min) > EPS) ?
                (v_max - v_min) / (v_max + v_min) : (v_max - v_min) / (2 - (v_max + v_min));

            double h = -1.;

            if (std::abs(g - r) < EPS && std::abs(g - b) < EPS)
                h = 0.;
            else
            {
                if (std::abs(v_max - r) < EPS)
                {
                    h = 60. * (g - b) / (v_max - v_min);
                }
                else if (std::abs(v_max - g) < EPS)
                {
                    h = 120. + 60. * (b - r) / (v_max - v_min);
                }
                else if (std::abs(v_max - b) < EPS)
                {
                    h = 240. + 60. * (r - g) / (v_max - v_min);
                }
            }
            if (h < 0.) h += 360.;
            newRow.emplace_back(h / 2., l * 255., s * 255.);
            });
        newValuesResource.emplace_back(newRow);
        });
    return HLS(newValuesResource);
}

RGB::operator Lab() const
{
    return static_cast<Lab>(static_cast<XYZ>(*this));
}

XYZ::operator Lab() const
{
    vector<vector<Point<double>>> newValuesResource;
    newValuesResource.reserve(resource.size());

    const double Z_n = 1.088754;
    const double X_n = 0.950456;

    std::for_each(resource.begin(), resource.end(), [&](const std::vector<Point<double>> row) {
        vector<Point<double>> newRow; newRow.reserve(row.size());

        std::for_each(row.begin(), row.end(), [&](Point<double> point) {

            std::tuple<double, double, double> point_tuple = static_cast<std::tuple<double, double, double>>(point);

            double x = std::get<0>(point_tuple); ;
            double y = std::get<1>(point_tuple); ;
            double z = std::get<2>(point_tuple); ;

            x /= X_n;
            z /= Z_n;
            double l = (y >= 0.008856 + EPS) ? 116. * std::pow(y, 1. / 3.) - 16. : 903.3 * y;
            double a = 500. * (f(x) - f(y));
            double b = 200. * (f(y) - f(z));

            newRow.emplace_back(l * 2.55, a + 128., b + 128.);
            });
        newValuesResource.emplace_back(newRow);
        });
    return Lab(newValuesResource);

}

XYZ::operator RGB() const
{
    vector<vector<Point<u_char>>> newValuesResource;
    newValuesResource.reserve(resource.size());

    std::for_each(resource.begin(), resource.end(), [&](const std::vector<Point<double>> row) {
        vector<Point<u_char>> newRow; newRow.reserve(row.size());

        std::for_each(row.begin(), row.end(), [&](Point<double> point) {

            std::tuple<double, double, double> point_tuple = static_cast<std::tuple<double, double, double>>(point);

            double x = std::get<0>(point_tuple);
            double y = std::get<1>(point_tuple);
            double z = std::get<2>(point_tuple);

            double r = 3.240479 * x - 1.53715 * y - 0.498535 * z;
            double g = -0.969256 * x + 1.875991 * y + 0.041556 * z;
            double b = 0.055648 * x - 0.204043 * y + 1.057311 * z;

            newRow.emplace_back(static_cast<u_char>(static_cast<u_int32_t>(std::round(r))),
                static_cast<u_char>(static_cast<u_int32_t>(std::round(g))),
                static_cast<u_char>(static_cast<u_int32_t>(std::round(b))));
            });
        newValuesResource.emplace_back(newRow);
        });
    return RGB(newValuesResource);
}

XYZ::operator HLS() const
{
    return static_cast<HLS>(static_cast<RGB>(*this));
}

XYZ::operator HSV() const
{
    return static_cast<HSV>(static_cast<RGB>(*this));
}

HSV::operator RGB() const
{
    vector<vector<Point<u_char>>> newValuesResource;
    newValuesResource.reserve(resource.size());

    std::for_each(resource.begin(), resource.end(), [&](const std::vector<Point<double>> row) {
        vector<Point<u_char>> newRow; newRow.reserve(row.size());

        std::for_each(row.begin(), row.end(), [&](Point<double> point) {

            std::tuple<double, double, double> point_tuple = static_cast<std::tuple<double, double, double>>(point);

            double h = std::get<0>(point_tuple) / 255.;
            double s = std::get<1>(point_tuple) / 255.;
            double v = std::get<2>(point_tuple) / 255.;

            double r = -1.;
            double g = -1.;
            double b = -1.;

            int i = static_cast<int>(std::floor(h * 6.));
            double f = h * 6. - i;
            double p = v * (1 - s);
            double q = v * (1 - f * s);
            double t = v * (1 - (1 - f) * s);

            switch (i % 6)
            {
            case 0:
                r = v; g = t; b = p;
                break;
            case 1:
                r = q; g = v; b = t;
                break;
            case 2:
                r = p; g = v; b = t;
                break;
            case 3:
                r = p; g = q; b = v;
                break;
            case 4:
                r = t; g = p; b = v;
                break;
            case 5:
                r = v; g = p; b = q;
                break;
            }
            newRow.emplace_back(static_cast<u_char>(static_cast<u_int32_t>(std::round(r * 255.))),
                static_cast<u_char>(static_cast<u_int32_t>(std::round(g * 255.))),
                static_cast<u_char>(static_cast<u_int32_t>(std::round(b * 255.))));
            });
        newValuesResource.emplace_back(newRow);
        });
    return RGB(newValuesResource);
}

HSV::operator HLS() const
{
    return static_cast<HLS>(static_cast<RGB>(*this));
}

HSV::operator XYZ() const
{
    return static_cast<XYZ>(static_cast<RGB>(*this));
}

HSV::operator Lab() const
{
    return static_cast<Lab>(static_cast<RGB>(*this));
}

HLS::operator RGB() const
{
    vector<vector<Point<u_char>>> newValuesResource;
    newValuesResource.reserve(resource.size());

    std::for_each(resource.begin(), resource.end(), [&](const std::vector<Point<double>> row) {
        vector<Point<u_char>> newRow; newRow.reserve(row.size());

        std::for_each(row.begin(), row.end(), [&](Point<double> point) {

            std::tuple<double, double, double> point_tuple = static_cast<std::tuple<double, double, double>>(point);

            double h = std::get<0>(point_tuple);
            double l = std::get<1>(point_tuple);
            double s = std::get<2>(point_tuple);

            double r = -1.;
            double g = -1.;
            double b = -1.;

            if (s <= EPS) { r = l; g = l; b = l; }
            else
            {
                double q = l < 0.5 ? l * (1 + s) : l + s - l * s;
                double p = 2 * l - q;
                std::function hueToRGB = [&](double t) {
                    if (t < 0) t += 1;
                    if (t > 1) t -= 1;
                    if (t < 1 / 6) return p + (q - p) * 6 * t;
                    if (t < 1 / 2) return q;
                    if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
                    return p;
                    };
                r = hueToRGB(h + 1. / 3.);
                g = hueToRGB(h);
                b = hueToRGB(h - 1. / 3.);

            }
            newRow.emplace_back(static_cast<u_char>(static_cast<u_int32_t>(std::round(r))),
                static_cast<u_char>(static_cast<u_int32_t>(std::round(g))),
                static_cast<u_char>(static_cast<u_int32_t>(std::round(b))));
            });
        newValuesResource.emplace_back(newRow);
        });
    return RGB(newValuesResource);
}

HLS::operator HSV() const
{
    return static_cast<HSV>(static_cast<RGB>(*this));
}

HLS::operator XYZ() const
{
    return static_cast<XYZ>(static_cast<RGB>(*this));
}

HLS::operator Lab() const
{
    return static_cast<Lab>(static_cast<RGB>(*this));
}

Lab::operator XYZ() const
{
    vector<vector<Point<double>>> newValuesResource;
    newValuesResource.reserve(resource.size());

    const double Z_n = 108.8754;
    const double X_n = 95.0456;

    std::for_each(resource.begin(), resource.end(), [&](const std::vector<Point<double>> row) {
        vector<Point<double>> newRow; newRow.reserve(row.size());

        std::for_each(row.begin(), row.end(), [&](Point<double> point) {

            std::tuple<double, double, double> point_tuple = static_cast<std::tuple<double, double, double>>(point);

            double l = std::get<0>(point_tuple);
            double a = std::get<1>(point_tuple); a -= 128.;
            double b = std::get<2>(point_tuple); b -= 128.;

            double y = (l + 16.) / 116.;
            double x = (a / 500.) + y;
            double z = y - (b / 200.);

            std::function checkVal = [](double t) {
                return t >= ((6. / 29.) + EPS) ? std::pow(t, 3.) : (3. * (6. / 29.) * (6. / 29.) * (t - (4. / 29.)));
                };

            newRow.emplace_back(checkVal(x) * X_n / 100., checkVal(y), checkVal(z) * Z_n / 100.);
            });
        newValuesResource.emplace_back(newRow);
        });
    return XYZ(newValuesResource);
}

Lab::operator RGB() const
{
    return static_cast<RGB>(static_cast<XYZ>(*this));
}

Lab::operator HLS() const
{
    return static_cast<HLS>(static_cast<XYZ>(*this));
}

Lab::operator HSV() const
{
    return static_cast<HSV>(static_cast<XYZ>(*this));
}

RGB::operator CMYK() const
{
    vector<vector<Point4<u_char>>> newValuesResource;
    newValuesResource.reserve(resource.size());

    std::for_each(resource.begin(), resource.end(), [&](const std::vector<Point<u_char>> row) {

        vector<Point4<u_char>> newRow; newRow.reserve(row.size());

        std::for_each(row.begin(), row.end(), [&](Point<u_char> point) {

            std::tuple<u_char, u_char, u_char> point_tuple = static_cast<std::tuple< u_char, u_char, u_char>>(point);

            double r = static_cast<double>(std::get<0>(point_tuple)); r /= 255.;
            double g = static_cast<double>(std::get<1>(point_tuple)); g /= 255.;
            double b = static_cast<double>(std::get<2>(point_tuple)); b /= 255.;

            double k = (1. - std::max(r, std::max(g, b)));
            double c, m, y;
            if (std::abs(1. - k) < EPS)
            {
                c = 0.; m = 0.; y = 0.;
            }
            else
            {
                c = 100. * (1. - r - k) / (1. - k);
                m = 100. * (1. - g - k) / (1. - k);
                y = 100. * (1. - b - k) / (1. - k);

            }

            std::cout << r << " " << g << " " << b << "\n";
            std::cout << c << " " << m << " " << y << ' ' << k << "\n";
            newRow.emplace_back(static_cast<u_char>(static_cast<u_int32_t>(std::round(c))),
                static_cast<u_char>(static_cast<u_int32_t>(std::round(m))),
                static_cast<u_char>(static_cast<u_int32_t>(std::round(y))),
                static_cast<u_char>(static_cast<u_int32_t>(std::round(100. * k))));
            });
        newValuesResource.emplace_back(newRow);
        });
    return CMYK(newValuesResource);
}

HLS::operator CMYK() const
{
    return static_cast<CMYK>(static_cast<RGB>(*this));
}

HSV::operator CMYK() const
{
    return static_cast<CMYK>(static_cast<RGB>(*this));
}

Lab::operator CMYK() const
{
    return static_cast<CMYK>(static_cast<RGB>(*this));
}

XYZ::operator CMYK() const
{
    return static_cast<CMYK>(static_cast<RGB>(*this));
}

CMYK::operator RGB() const
{
    vector<vector<Point<u_char>>> newValuesResource;
    newValuesResource.reserve(resource.size());

    std::for_each(resource.begin(), resource.end(), [&](const std::vector<Point4<u_char>> row) {

        vector<Point<u_char>> newRow; newRow.reserve(row.size());

        std::for_each(row.begin(), row.end(), [&](Point4<u_char> point) {

            std::tuple<u_char, u_char, u_char, u_char> point_tuple = static_cast<std::tuple<u_char, u_char, u_char, u_char>>(point);

            double c = static_cast<double>(std::get<0>(point_tuple)); c /= 100.;
            double m = static_cast<double>(std::get<1>(point_tuple)); m /= 100.;
            double y = static_cast<double>(std::get<2>(point_tuple)); y /= 100.;
            double k = static_cast<double>(std::get<3>(point_tuple)); k /= 100.;

            double r = 255. * (1. - c) / (1. - k);
            double g = 255. * (1. - m) / (1. - k);
            double b = 255. * (1. - y) / (1. - k);

            newRow.emplace_back(static_cast<u_char>(static_cast<u_int32_t>(std::round(r))),
                static_cast<u_char>(static_cast<u_int32_t>(std::round(g))),
                static_cast<u_char>(static_cast<u_int32_t>(std::round(b))));
            });
        newValuesResource.emplace_back(newRow);
        });
    return RGB(newValuesResource);
}

CMYK::operator Lab() const
{
    return static_cast<Lab>(static_cast<RGB>(*this));
}
CMYK::operator HSV() const
{
    return static_cast<HSV>(static_cast<RGB>(*this));
}
CMYK::operator HLS() const
{
    return static_cast<HLS>(static_cast<RGB>(*this));
}
CMYK::operator XYZ() const
{
    return static_cast<XYZ>(static_cast<RGB>(*this));
}