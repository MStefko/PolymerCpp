/* Classes for simulating random walk models for polymer
 * physics DNA/chromatin studies
 */

#include <iostream>      // Input/output
#include <Eigen/Core>    // Linear algebra
#include <cmath>         // General math operations
#include <cstdlib>       // Standard library
#include <ctime>         // Timing
#include <string>        // Strings
#include <vector>        // Vectors for storing data
#include <random>        // Generating random numbers
#include <stdexcept>     // Throwing exceptions


using namespace std;


#define pi 3.1415926

// defines engine and distribution for random number generation
std::default_random_engine randGenerator;
std::uniform_real_distribution<double> randUniformReal(0.0,1.0);
std::normal_distribution<double> randNormalReal(0.0,1.0);


class Path
{
private:
    
public:
    Eigen::Vector3d * points;

    Eigen::Vector3d * randPointSphere(int numPoints)
    /* Randomly select points from the surface of a sphere.
     * Parameters
     * ----------
     * numPoints : int
     * The number of points to return.
     *
     * Returns
     * -------
     * points : array of 3D vectors
     *    The x, y, and z coordinates of each point on the sphere.
     *
     * References
     * ----------
     * [1] Weisstein, Eric W. "Sphere Point Picking." From
     * MathWorld--A Wolfram Web
     * Resource. http://mathworld.wolfram.com/SpherePointPicking.html
     */
    {
        Eigen::Vector3d * points = new Eigen::Vector3d[numPoints];
        for (int i=0; i<numPoints; i++) {
            
            // generate 2 random numbers and transform them into angles
            double phi = 2 * pi * randUniformReal(randGenerator);
            double theta = acos(2 * randUniformReal(randGenerator) - 1);
            
            // convert to cartesian coordinates and fill vector
            points[i](0) = cos(phi) * sin(theta);
            points[i](1) = sin(phi) * sin(theta);
            points[i](2) = cos(theta);
        }
        return points;
    }

    void checkPath()
    /* Checks that a path has the correct number of columns
     * NOT YET IMPLEMENTED
     */
    {
        return;
    }
};

class WormlikeChain: public Path
/* Creates a 3D wormlike chain.
 * 
 * Parameters:
 * -----------
 * numSegments : float
 *     The number of segments in the chain. Must be >=1
 *     and need not be an integer.
 * pLength: int
 *     The persistence length in units of chain segments.
 * initPoint : Vector3d, optional
 *     The coordinates of the first point of the chain.
 *
 * Attributes:
 * -----------
 * path : <vector> of Vector3d points
 *     vector, whose elements are 3d vectors describing
 *     endpoints of the segments comprising the path
 */
{

private: 
    double numSegments;
    int pLength;
    Eigen::Vector3d initPoint;
    vector<Eigen::Vector3d> path;


public:
    WormlikeChain(double in_numSegments, int in_pLength,
                  Eigen::Vector3d * in_initPoint)
    {
        numSegments = in_numSegments;
        pLength = in_pLength;
        initPoint = *in_initPoint;
    }

    void makePath()
    /*  Create the wormlike chain.
     *  
     *  The wormlike chain is created by first choosing the sizes of
     *  the small, random displacements in a plane tangent to a point
     *  on the surface of the unit sphere defined by the vector
     *  currPoint. The distribution for the sizes is given by the
     *  Boltzmann statistics for a semiflexible rod bending by a given
     *  angle due to interaction with its thermal environment.
     *  
     *  A random direction in this tangent plane is chosen by randomly
     *  and uniformly generating a vector on the unit sphere, taking
     *  its cross product with the currPoint vector, and normalizing
     *  the cross product. This cross product is multiplied by the
     *  size of the displacement found previously to generate the
     *  displacement vector.
     *  
     *  After displacing the currPoint vector into the tangent plane,
     *  the point in the plane is back projected onto the unit sphere
     *  to find the vector representing the next step in the polymer
     *  walk.
     *  
     *  This process is repeated until a number of vectors determined
     *  by numSegments representing a random walk on the surface of a
     *  sphere are generated. These vectors are cumulatively summed at
     *  the end to produce the final path field, which is the
     *  trajectory of the polymer.
     *
     *  Initial point to start polymer is determined by initPoint,
     *  which is set when initializing the class WormlikeChain.
     */
    {
        // check if numSegments is in valid range
        if (numSegments < 1)
        {
            std::stringstream buffer;
            buffer << "The number of segments must be greater than 1, but a "
                << "value of " << numSegments << "was supplied." << std::endl;
            throw std::out_of_range(buffer.str());
        }

        // split numSegments into integer and leftover
        int numSegInt; double numSegFrac;
        numSegInt = (int)numSegments;
        numSegFrac = numSegments - (float)numSegInt;

        // If numSegFrac is not zero, allocates memory also for leftover vector
        int numSeg = (abs(numSegFrac)>0.001) ? numSegInt : numSegInt-1;

        // Create the displacement distances in the tangent planes
        double *angDisp = new double[numSeg];
        double *tanPlaneDisp = new double[numSeg];
        double sigma = pow(2.0 / (float)pLength, 0.5);
        for (int i=0; i<numSeg; i++)
        {
            angDisp[i] = sigma * randNormalReal(randGenerator);
            tanPlaneDisp[i] = sin(angDisp[i]);
        }

        // Create random vectors uniformly sampled from the unit sphere
        Eigen::Vector3d * randVecs;
        randVecs = randPointSphere(numSeg);

        // Final small displacement for non-integer numSegments
        if (abs(numSegFrac) > 0.001)
        {
            angDisp[numSeg-1] = pow(pLength*numSegFrac,0.5) *
                           randNormalReal(randGenerator);
            tanPlaneDisp[numSeg-1] = numSegFrac * sin(angDisp[numSeg-1]);
            randVecs[numSeg-1] = numSegFrac * randVecs[numSeg-1];
        }

        // Primary iterative loop for creating the chain
        Eigen::Vector3d currPoint = initPoint;
        Eigen::Vector3d * workingPath = new Eigen::Vector3d[numSeg];
        Eigen::Vector3d dispVector;
        for (int i=0; i<numSeg; i++)
        {
            // Create a displacement in the plane tangent to currPoint
            dispVector = currPoint.cross(randVecs[i]);

            // Check if displacement and currPoint vectors are parallel
            while (dispVector.squaredNorm()<0.01)
        }

    }

    Eigen::Vector3d getInitPoint()
    {
        return initPoint;
    }
};

/* Class which stores date and time in formatted strings at the time
 * of its initialization */
class DateClass
{
private:
    string m_date;
    string m_time;
    time_t tick;
    struct tm * timeinfo;

public:
    DateClass() // constructor
    {
        tick = std::time(&tick);
        timeinfo = localtime (&tick);
        char date[20], watch[20];
        strftime(date, 20, "%y-%m-%d", timeinfo);
        strftime(watch, 20, "%H-%M-%S", timeinfo);
        m_date.assign(date);
        m_time.assign(watch);
    }
    /* redundant constructor, delete?
    DateClass(time_t tock)
    {
        char date[20], watch[20];
        timeinfo = localtime (&tock);
        strftime(date, 20, "%y-%m-%d", timeinfo);
        strftime(watch, 20, "%H-%M-%S", timeinfo);
        m_date.assign(date);
        m_time.assign(watch);
    }
    */
    string GetDate()
    {
        return m_date;
    }
    string GetTime()
    {
        return m_time;
    }
};


int main (int argc, char *argv[])
{
    DateClass DateTimeOnLaunch;  // Get time information for naming the database
    cout << "Today is " << DateTimeOnLaunch.GetDate() << ".\n";
    Eigen::Vector3d basePoint(8.,6.,5.);
    WormlikeChain WLC(3.,2.,&basePoint);
    cout << "Generating 10000 random sphere vectors.\n";
    WLC.points = WLC.randPointSphere(10000);
    for (int i=0; i<10000; i++)
    {
        printf("%f\t%f\t%f\n",WLC.points[i](0),WLC.points[i](1),
            WLC.points[i](2));
    }
    cout << "basePoint of WLC is: " << WLC.getInitPoint();
}
