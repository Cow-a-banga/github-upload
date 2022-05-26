#include <math.h>
#include <fstream>

using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Helper class implementing 3D vector and corresponding basic operations.
////////////////////////////////////////////////////////////////////////////////

class vec3
{
    float m_x, m_y, m_z;

public:
    vec3(float x, float y, float z): m_x(x), m_y(y), m_z(z) {};
    vec3(): m_x(0), m_y(0), m_z(0) {};

    vec3 operator +(const vec3 &v) const { return vec3(m_x + v.x(), m_y + v.y(), m_z + v.z()); };
    vec3 operator -(const vec3 &v) const { return vec3(m_x - v.x(), m_y - v.y(), m_z - v.z()); };
    vec3 operator *(float t) const { return vec3(m_x * t, m_y * t, m_z * t); };
    void operator +=(const vec3 &v) { m_x += v.x(); m_y += v.y(); m_z += v.z(); };
    void operator *=(float t) { m_x *= t; m_y *= t; m_z *= t; };

    float x() const { return m_x; };
    float y() const { return m_y; };
    float z() const { return m_z; };

    void set(float x, float y, float z) { m_x = x; m_y = y; m_z = z; };

    float length() const
    {
        return sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
    };
    void normalize()
    {
        float l = length();
        m_x /= l;
        m_y /= l;
        m_z /= l;
    };
    float dot(const vec3 &v) const
    {
        return m_x * v.x() + m_y * v.y() + m_z * v.z();
    };
    vec3 cross(const vec3 &v) const
    {
        return vec3(m_y * v.z() - m_z * v.y(), m_z * v.x() - m_x * v.z(), m_x * v.y() - m_y * v.x());
    };
    void applyGamma(float gamma)
    {
        m_x = pow(m_x, gamma);
        m_y = pow(m_y, gamma);
        m_z = pow(m_z, gamma);
    };
};

////////////////////////////////////////////////////////////////////////////////
// Helper functions.
////////////////////////////////////////////////////////////////////////////////

float clamp(float v, float from, float to)
{
    if (v < from)
        return from;
    else if (v > to)
        return to;
    else
        return v;
}

float frandom()
{
    return (float)(rand() % 1000) / 1500.0f;
}

float min(float a, float b)
{
    return a < b ? a : b;
}

float max(float a, float b)
{
    return a > b ? a : b;
}

////////////////////////////////////////////////////////////////////////////////
// Visualization functions.
////////////////////////////////////////////////////////////////////////////////

/**
 * Estimate the distance from the given point to the nearset bound of the 3D projection of mandelbulb set.
 * For details about the madelbulb set, see: https://www.skytopia.com/project/fractal/mandelbulb.html
 * For details about the distance estimation technique, see: http://blog.hvidtfeldts.net/index.php/2011/09/distance-estimated-3d-fractals-v-the-mandelbulb-different-de-approximations/
 *
 * @param pos - position in 3D space from which to estimate the distance.
 * @param orbitTrap - optional info for calculating the colors, for details, see https://www.iquilezles.org/www/articles/ftrapsgeometric/ftrapsgeometric.htm
 * @return estimated distance.
 */
float mandelbulb(const vec3 &pos, vec3 *orbitTrap = nullptr)
{
    float threshold = pos.length() - 1.2f;
    if (threshold > 0.2f)
        return threshold;

    // Z_i+1 = Z_i^8 + C
    const float power = 8.0f;
    vec3 z = pos;
    vec3 c = pos;

    if (orbitTrap)
        orbitTrap->set(1.0e20f, 1.0e20f, 1.0e20f);

    float dr = 1.0f;
    float r = 0.0f;
    for (int i = 0; i < 100; ++i)
    {
        r = z.length();
        if (r > 2.0f)
            break;
        float theta = acos(z.z() / r);
        float phi = atan2(z.y(), z.x());

        dr = pow(r, power - 1.0f) * power * dr + 1.0f;

        float zr = pow(r, power);
        theta *= power;
        phi *= power;

        z = vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta)) * zr;
        z += c;

        if (orbitTrap)
        {
            orbitTrap->set(min(pow(abs(z.z()), 0.1f), orbitTrap->x()),
                           min(abs(z.x()) - 0.15f, orbitTrap->y()),
                           min(z.length(), orbitTrap->z()));
        }
    }

    return 0.5f * log(r) * r / dr;
}

/**
 * Cast the day from given camera position in given direction and search for the distance where this ray hits the bound of mandelbulb set projection.
 *
 * @param camPos - position of camera.
 * @param ray - direction of the ray.
 * @param orbitTrap - optional info for calculating the colors, for details, see https://www.iquilezles.org/www/articles/ftrapsgeometric/ftrapsgeometric.htm
 * @return distance to the nearest bound of mandelbulb set projection or -1 if no hit found.
 */
float castRay(const vec3 &camPos, const vec3 &ray, vec3 *orbitTrap)
{
    const float maxT = 200.0f;
    float t = 0.0f;
    for (int i = 0; i < 100; ++i)
    {
        vec3 pos = camPos + ray * t;
        float h = mandelbulb(pos, orbitTrap);
        if (h < 0.0003f)
            break;
        t += h;
        if (t > maxT)
        {
            t = -1.0f;
            break;
        }
    }
    return t;
}

/**
 * Calculate normal to the bound of mandelbulb set projection in given point using tetrahedron technique to calculate density function gradient.
 * For details, see http://iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
 *
 * @param pos - position in 3D space where to calculate the normal.
 * @return calculated normal.
 */
vec3 calcNormal(const vec3 &pos)
{
    const float h = 0.0001f;
    vec3 normal = vec3(1.0f, -1.0f, -1.0f) * mandelbulb(pos + vec3(h, -h, -h)) +
                  vec3(-1.0f, -1.0f, 1.0f) * mandelbulb(pos + vec3(-h, -h, h)) +
                  vec3(-1.0f, 1.0f, -1.0f) * mandelbulb(pos + vec3(-h, h, -h)) +
                  vec3(1.0f, 1.0f, 1.0f) * mandelbulb(pos + vec3(h, h, h));
    normal.normalize();
    return normal;
}

/**
 * Estimate the ambient occlusion of the given point of mandelbulb set projection bound.
 * For details, see https://en.wikipedia.org/wiki/Ambient_occlusion
 *
 * @param pos - position in 3D space where to estimate ambient occlusion.
 * @param normal - normal in that point.
 * @param fadeout - the light fadeout coefficient.
 * @return estimated light intencity in the given point with respect to ambient occlusion.
 */
float ambientOcclusion(const vec3 &pos, const vec3 &normal, float fadeout)
{
    const int sampleCount = 12;
    const float maxDistance = 0.07f;

    float diff = 0.0f;
    for (int i = 0; i < sampleCount; ++i)
    {
        float distance = maxDistance * frandom();
        float sampleAtDistance = max(0.0f, mandelbulb(pos + normal * distance));
        diff += (distance - sampleAtDistance) / maxDistance;
    }

    float diffNorm = diff / (float)sampleCount;

    return 1.0f - diffNorm / fadeout;
}

////////////////////////////////////////////////////////////////////////////////
// Main.
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    const int width = 1280;
    const int height = 720;
    const int AA = 2; // Number of samples for antialiasing.
    const vec3 baseColor1(0.906f, 0.929f, 0.922f); // Color palette, color 1.
    const vec3 baseColor2(0.0f, 0.227f, 0.42f); // Color palette, color 2.
    const vec3 baseColor3(0.259f, 0.765f, 0.969f); // Color palette, color 3.
    const vec3 lightPos(2.0f, -2.0f, 2.0f); // Position of light source in 3D space.
    const float lightIntensity = 4.0f; // Strength of light source.
    const vec3 camPos(0.0f, -9.0f, -9.0f); // Position of camera in 3D space.
    const vec3 camTarget(0.0f, 0.0f, 0.0f); // Point the camera looks at.

    char *image = new char[width * height * 3];

    // Create coordinate systemm of a camera.
    vec3 camZ = camTarget - camPos;
    camZ.normalize();

    vec3 camX = vec3(0.0f, 1.0f, 0.0f).cross(camZ);
    camX.normalize();

    vec3 camY = camZ.cross(camX);
    camY.normalize();

    // Main loops.
    for (int y = 0, idx = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            vec3 fragColor;
            // Antialiasing loops.
            for (int aaY = 0; aaY < AA; ++aaY)
            {
                for (int aaX = 0; aaX < AA; ++aaX)
                {
                    // Cast a ray from the camera through the screen point inside the 3D scene.
                    vec3 point((2.0f * ((float)x + (float)aaX / (float)AA) - (float)width) / (float)height,
                               (2.0f * ((float)y + (float)aaY / (float)AA) - (float)height) / (float)height,
                               0.0f);
                    vec3 ray = camX * point.x() + camY * point.y() + camZ - camPos;
                    ray.normalize();
                    vec3 orbitTrap;
                    float t = castRay(camPos, ray, &orbitTrap);
                    if (t > 0.0f) // If ray hits mandelbulb set projection bound.
                    {
                        // Calculate the base color.
                        vec3 color = baseColor1 * clamp(pow(orbitTrap.x(), 20.0f), 0.0f, 1.0f) +
                                     baseColor2 * clamp(pow(orbitTrap.y(), 20.0f), 0.0f, 1.0f) +
                                     baseColor3 * clamp(pow(orbitTrap.z(), 20.0f), 0.0f, 1.0f);
                        // We could eventually stop here, but we want shading effects. So, first calculate the surface normal.
                        vec3 p = camPos + ray * t;
                        vec3 normal = calcNormal(p);
                        // Then estimate the ambient occlusion and fade the color accordingly.
                        float ao = ambientOcclusion(p, normal, 0.46f);
                        color *= ao + 0.01f;
                        // Then calculate the lambertian shading and fade the color accordingly.
                        vec3 l = lightPos - p;
                        l.normalize();
                        color *= max(normal.dot(l), 0.1f) * lightIntensity;
                        // Now apply sRGB gamma correction to ensure correct displaying of the result color.
                        color.applyGamma(1.0f / 2.2f);
                        // Last, increase contrast to make things look better.
                        color.set(1.1f * (color.x() - 0.5f) + 0.5f,
                                  1.1f * (color.y() - 0.5f) + 0.5f,
                                  1.1f * (color.z() - 0.5f) + 0.5f);
                        // And finally accumulate the color for antialiasing.
                        fragColor += color;
                    }
                }
            }
            // Normalize accumulated color for antialiasing.
            fragColor *= 1.0f / (float)(AA * AA);
            // Store the color in the image.
            image[idx++] = (char)(clamp(fragColor.z() * 255.0f, 0.0f, 255.0f));
            image[idx++] = (char)(clamp(fragColor.y() * 255.0f, 0.0f, 255.0f));
            image[idx++] = (char)(clamp(fragColor.x() * 255.0f, 0.0f, 255.0f));
        }
    }

    // Write out the result.
    fstream tgaFile("out.tga", ios::out | ios::binary);
    const char header[] =
            {
                    0x0, 0x0, 0x2, 0x0, 0x0,
                    0x0, 0x0, 0x0, 0x0, 0x0,
                    0x0, 0x0, width & 0xFF, (width >> 8) & 0xFF,
                    height & 0xFF, (height >> 8) & 0xFF, 24, 0x0
            };
    tgaFile.write(header, sizeof(header));
    tgaFile.write(image, width * height * 3);
    tgaFile.close();

    delete[] image;

    return 0;
}