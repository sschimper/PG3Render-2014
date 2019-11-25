#pragma once 

#include "math.hxx"
#include "rng.hxx"

class Material
{
public:
    Material()
    {
        Reset();
    }

    void Reset()
    {
        mDiffuseReflectance = Vec3f(0);
        mPhongReflectance   = Vec3f(0);
        mPhongExponent      = 1.f;
    }

	// function that returns the maximum component in a vector
	float getMaxElementInVector(Vec3f vector) const
	{
		float x = vector.x;
		float y = vector.y;
		float z = vector.z;

		float coordinates[] = { x,y,z };
		float max = coordinates[0];

		for (int i = 1; i < 3; i++) {
			if (coordinates[i] >= max)
				max = coordinates[i];
		}
		return max;
	}

	// generate random ray direction on hemisphere
	Vec3f sampleDiffuse(float &r1, float &r2) const
	{
		float sinTheta = sqrt(1 - r2);
		float cosTheta = sqrt(r2);
		float phi = (float) 2.0*PI_F*r1;

		// convert [theta, phi] to Cartesian coordinates
		Vec3f dir(cos(phi)*sinTheta, sin(phi)*sinTheta, cosTheta);

		return dir;
	}

	// create random direction on "glossy" lobe
	Vec3f sampleGlossy(const Vec3f& wog, Vec3f normal, float &r1, float &r2) const
	{
		Vec3f idealReflected = 2 * Dot(wog, normal) * normal - wog; // ideal reflected direction

		Frame reflectionFrame;
		reflectionFrame.SetFromZ(idealReflected);

		// generate direction in local coordinate frame
		Vec3f locDir = rndHemiCosN(r1, r2);  // formulas form prev. slide

		Vec3f dir = reflectionFrame.ToWorld(locDir);

		return dir;
	}

	// sample rnd point on sphere
	Vec3f rndHemiCosN(float &r1, float &r2) const
	{
		float zExp = 1.f / (mPhongExponent + 1);
		float sqrtTerm = std::sqrt(std::max(0.f, 1.f - pow(r2, 2 * zExp)));
		float phi = 2 * PI_F * r1;

		return Vec3f(cos(phi) * sqrtTerm, sin(phi) * sqrtTerm, pow(r2, zExp));
	}

	// generate defuse pdf value
	float getPDFDiffuseValue(Vec3f genDir, Vec3f normal) const {
		return std::max(0.f, Dot(genDir, normal) * (1 / PI_F));
	}

	// generate glossy pdf value
	float getPDFGlossyValue(Vec3f wog, Vec3f normal, Vec3f genDir) const {

		// build a local coordinate frame with R = z-axis 
		Vec3f idealReflected = 2 * Dot(wog, normal) * normal - wog; // ideal reflected direction
		float cosTheta = std::max(0.f, Dot(idealReflected, genDir));

		return ((mPhongExponent + 1) / (2 * PI_F)) * std::pow(cosTheta, mPhongExponent);
	}


	Vec3f evalBrdf( const Vec3f& wil, const Vec3f& wol ) const
	{
		if( wil.z <= 0 && wol.z <= 0)
			return Vec3f(0);

		// calculate cos theta
		float cos_theta = calculateCosTheta(wil, wol);

		// formulars from slides
		Vec3f diffuseComponent = mDiffuseReflectance / PI_F;
		Vec3f glossyComponent = ((mPhongExponent + 2) / (2 * PI_F)) * mPhongReflectance * pow(cos_theta, mPhongExponent);

		return diffuseComponent + glossyComponent;
	}

	// selection of the BRDF component
	float evalBrdfPdf(Vec3f wog, Vec3f genDir, Vec3f normal) const
	{
		float pd = getMaxElementInVector(mDiffuseReflectance);
		float ps = getMaxElementInVector(mPhongReflectance);
		float sumPdPs = (pd + ps);
		pd /= sumPdPs;	 // prob of choosing the diffuse component
		ps /= sumPdPs;	 // prob of choosing the specular comp.

		return pd * getPDFDiffuseValue(genDir, normal) + ps * getPDFGlossyValue(wog, normal, genDir);

	}

	// calculate cos theta
	float calculateCosTheta(const Vec3f & wil, const Vec3f & wol) const
	{
		Vec3f normal = Vec3f(0, 0, 1);
		Vec3f reflection = 2 * Dot(wil, normal) * normal - wil;
		float this_cos_theta = Dot(reflection, wol);
		return this_cos_theta;
	}

    Vec3f mDiffuseReflectance;
    Vec3f mPhongReflectance;
    float mPhongExponent;
	Rng mRng;
};
