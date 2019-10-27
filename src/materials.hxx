#pragma once 

#include "math.hxx"

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
};
