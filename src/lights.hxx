#pragma once

#include <vector>
#include <cmath>
#include "math.hxx"
#include "rng.hxx"
#include "utils.hxx"

class AbstractLight
{
public:

	virtual Vec3f sampleIllumination(const Vec3f rndGen, const Vec3f& aSurfPt, const Frame& aFrame, Vec3f& oWig, float& oLightDist) const
	{
		return Vec3f(0);
	}

	virtual Vec3f getRadiance() const 
	{
		return Vec3f(0);
	}

	virtual float getPDF(float lightDist, Vec3f wig) const
	{
		return 0;
	}
};


//////////////////////////////////////////////////////////////////////////
class AreaLight : public AbstractLight
{
public:

    AreaLight(
        const Vec3f &aP0,
        const Vec3f &aP1,
        const Vec3f &aP2)
    {
        p0 = aP0;
        e1 = aP1 - aP0;
        e2 = aP2 - aP0;

        Vec3f normal = Cross(e1, e2);
        float len    = normal.Length();
        mInvArea     = 2.f / len;
        mFrame.SetFromZ(normal);
    }

	virtual Vec3f getRadiance() const
	{
		return mRadiance;
	}

	virtual Vec3f sampleIllumination(
		const Vec3f rndGen, 
		const Vec3f& aSurfPt,
		const Frame& aFrame,
		Vec3f& oWig,
		float& oLightDist) const
	{
		// get random x and y coordinate
		float areaX = rndGen.x;
		float areaY = rndGen.y;

		// make sure point (x,y) will lie inside the area light 
		if (areaX + areaY >= 1)
		{
			areaX = 1 - areaX;
			areaY = 1 - areaY;
		}

		// sample the point on the area
		Vec3f sampledPoint = p0 + (areaX * e1) + (areaY * e2);	

		oWig = sampledPoint - aSurfPt; // distance from "light point" to surface point
		float distSqr = oWig.LenSqr(); // distance squared
		oLightDist = sqrt(distSqr);

		oWig /= oLightDist;

		float cosThetaX = Dot(aFrame.mZ, oWig); // at surface point
		float cosThetaY = Dot(mFrame.mZ, -oWig); // at light source

		if (cosThetaX <= 0)
			return Vec3f(0);

		if (cosThetaY <= 0)
			return Vec3f(0);

		return mRadiance * (cosThetaX * cosThetaY) / (distSqr * mInvArea);
	}

	virtual float getPDF(float lightDist, Vec3f wig) const
	{
		float cosine = Dot(mFrame.mZ, -wig);
		
		// without this statement, light box would have unplausible 
		// "light stipe" in the box
		if (cosine < 0)
		{
			cosine = 0;
		}
		
		return (lightDist * lightDist) * mInvArea / cosine;
	}

public:
    Vec3f p0, e1, e2;
    Frame mFrame;
    Vec3f mRadiance;
    float mInvArea;
};

//////////////////////////////////////////////////////////////////////////
class PointLight : public AbstractLight
{
public:

    PointLight(const Vec3f& aPosition)
    {
        mPosition = aPosition;
    }

	virtual Vec3f sampleIllumination(
		const Vec3f rndGen, 
		const Vec3f& aSurfPt, 
		const Frame& aFrame, 
		Vec3f& oWig, 
		float& oLightDist) const
	{
		oWig           = mPosition - aSurfPt;
		float distSqr  = oWig.LenSqr();
		oLightDist     = sqrt(distSqr);
		
		oWig /= oLightDist;

		float cosTheta = Dot(aFrame.mZ, oWig);

		if(cosTheta <= 0)
			return Vec3f(0);

		return mIntensity * cosTheta / distSqr;
	}

	virtual float getPDF(float lightDist, Vec3f wig) const
	{
		return 1;
	}

public:

    Vec3f mPosition;
    Vec3f mIntensity;
};


//////////////////////////////////////////////////////////////////////////
class BackgroundLight : public AbstractLight
{
public:
	BackgroundLight()
	{
		mBackgroundColor = Vec3f(135, 206, 250) / Vec3f(255.f);
	}

	virtual Vec3f getRadiance() const
	{
		return mBackgroundColor;
	}


	virtual Vec3f sampleIllumination(
		const Vec3f rndGen, 
		const Vec3f& aSurfPt,
		const Frame& aFrame,
		Vec3f& oWig,
		float& oLightDist) const
	{
		// find random point on sphere
		Vec3f randomPointOnSphere = Vec3f(rndGen.x, rndGen.y, rndGen.z);

		// sample point on surface
		// by using rational form of sphere
		Vec3f sampledPoint = Vec3f(cos(2 * PI_F * randomPointOnSphere.y) * sqrt(1 - (randomPointOnSphere.x * randomPointOnSphere.x)),
			sin(2 * PI_F*randomPointOnSphere.y)*sqrt(1 - (randomPointOnSphere.x * randomPointOnSphere.x)),
			randomPointOnSphere.x);

		oWig = Normalize(aFrame.ToWorld(sampledPoint));

		// set distance incredibly high
		oLightDist = std::numeric_limits<float>::max();

		float cosTheta = Dot(aFrame.mZ, oWig);

		if (cosTheta <= 0)
			return Vec3f(0);

		return mBackgroundColor * cosTheta * (2 * PI_F);
	}

	// virtual float getPDF(float lightDist, Vec3f wig) const
	virtual float getPDF() const
	{
		return (1 / 4 * PI_F);
	}

public:
	Vec3f mBackgroundColor;
};