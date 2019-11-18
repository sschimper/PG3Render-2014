#pragma once

#include <vector>
#include <cmath>
#include <omp.h>
#include <cassert>
#include "renderer.hxx"
#include "rng.hxx"

// Right now this is a copy of EyeLight renderer. The task is to change this 
// to a full-fledged path tracer.
class PathTracer : public AbstractRenderer
{
public:

	PathTracer(
		const Scene& aScene,
		int aSeed = 1234
	) :
		AbstractRenderer(aScene), mRng(aSeed)
	{}

	virtual void RunIteration(int aIteration)
	{
		const int resX = int(mScene.mCamera.mResolution.x);
		const int resY = int(mScene.mCamera.mResolution.y);

		for (int pixID = 0; pixID < resX * resY; pixID++)
		{
			//////////////////////////////////////////////////////////////////////////
			// Generate ray
			const int x = pixID % resX;
			const int y = pixID / resX;

			const Vec2f sample = Vec2f(float(x), float(y)) + mRng.GetVec2f();

			Ray   ray = mScene.mCamera.GenerateRay(sample);
			Isect isect;
			isect.dist = 1e36f;

			if (mScene.Intersect(ray, isect))
			{
				const Vec3f surfPt = ray.org + ray.dir * isect.dist;
				Frame frame;
				frame.SetFromZ(isect.normal);
				const Vec3f wog = -ray.dir;
				const Vec3f wol = frame.ToLocal(-ray.dir);

				Vec3f LoDirect = Vec3f(0);
				const Material& mat = mScene.GetMaterial(isect.matID);

				// if light source is intersected, add the light to the final image
				if (isect.lightID >= 0) {
					const AbstractLight *abstLight = mScene.GetLightPtr(isect.lightID);
					const AreaLight *areaLight = dynamic_cast<const AreaLight*>(abstLight);

					if (areaLight != 0) {
						mFramebuffer.AddColor(sample, areaLight->mRadiance);
						continue;
					}
				}

				/* ASSIGNMENT 1
				for (int i = 0; i < mScene.GetLightCount(); i++) // comment this out for Assignment II
				{
					const AbstractLight* light = mScene.GetLightPtr(i);
					assert(light != 0);

					Vec3f wig; float lightDist;
					Vec3f illum = light->sampleIllumination(mRng.GetVec3f(), surfPt, frame, wig, lightDist); // debug

					if (illum.Max() > 0)
					{
						if (!mScene.Occluded(surfPt, wig, lightDist))
							LoDirect += illum * mat.evalBrdf(frame.ToLocal(wig), wol);
					}
				}
				*/

				// set up for second ray
				Vec3f normal = Normalize(isect.normal); // normal at intersection point
				Vec3f genDir; // generated direction
				Ray secondRay; // second Ray
				Isect secondRayIsect; // second intersection
				float ps;
				float pd;

				// generate new direction
				createSecondRay(mat, genDir, secondRay, secondRayIsect, frame, wog, surfPt, normal, pd, ps);

				// evaluate Pdf
				float pdf = pd * mat.getPDFDiffuseValue(genDir, normal)
					+ ps * mat.getPDFGlossyValue(wog, normal, genDir);

				// if the ray hits a light source, ask the light to give the radiance ...
				if (mScene.Intersect(secondRay, secondRayIsect)) 
				{
					// works only for area light because it is mathematically 
					// impossible to hit a point that is infinitely small
					if (secondRayIsect.lightID >= 0) 
					{
						const AbstractLight *abstLight = mScene.GetLightPtr(secondRayIsect.lightID);
						float cosTheta = Dot(normal, genDir);

						LoDirect += (abstLight->getRadiance() * mat.evalBrdf(frame.ToLocal(genDir), wol) * cosTheta) / pdf;
					}
				}
				// ... and if there is no light source in the scene -> background light
				// ask the background light to give the radiance
				else if (mScene.GetBackground()) {
					Vec3f radiance = mScene.GetBackground()->mBackgroundColor;
					float cosTheta = Dot(normal, genDir);
					LoDirect += (radiance * mat.evalBrdf(frame.ToLocal(genDir), wol) * cosTheta) / pdf;
				}

				mFramebuffer.AddColor(sample, LoDirect); // finally add the information to the image


				/*
				float dotLN = Dot(isect.normal, -ray.dir);
				// this illustrates how to pick-up the material properties of the intersected surface
				const Material& mat = mScene.GetMaterial( isect.matID );
				const Vec3f& rhoD = mat.mDiffuseReflectance;
				// this illustrates how to pick-up the area source associated with the intersected surface
				const AbstractLight *light = isect.lightID < 0 ?  0 : mScene.GetLightPtr( isect.lightID );
				// we cannot do anything with the light because it has no interface right now
				if(dotLN > 0)
					mFramebuffer.AddColor(sample, (rhoD/PI_F) * Vec3f(dotLN));
				*/
			}
		}

		mIterations++;
	}

	// ASSIGNMENT 2
	// select a BRDF component and create a new random direction
	void createSecondRay(const Material & mat, 
									Vec3f &genDir, 
									Ray &secondRay,
									Isect &secondRayIsect,
									Frame &frame, 
									Vec3f wog, 
									Vec3f surfPt,
									Vec3f &normal,
									float &pd,
									float &ps)
	{
		// generate new direction
		pd = mat.getMaxElementInVector(mat.mDiffuseReflectance);
		ps = mat.getMaxElementInVector(mat.mPhongReflectance);
		float sumPdPs = (pd + ps);
		pd /= sumPdPs;	 // prob of choosing the diffuse component
		ps /= sumPdPs;	 // prob of choosing the specular comp.

		float r1 = mRng.GetFloat();
		float r2 = mRng.GetFloat();

		if (mRng.GetFloat() <= pd) {
			genDir = frame.ToWorld(mat.sampleDiffuse(r1, r2));
		}
		else {
			genDir = mat.sampleGlossy(wog, normal, r1, r2);
		}

		// instantiate following ray
		secondRay.dir = genDir;
		secondRay.org = surfPt + genDir * EPS_RAY; // move it a bit in the generated dorection and cast ray
		secondRay.tmin = 0;

		secondRayIsect.dist = 1e36f; // distance from starting point to intersection 
	}

	Rng              mRng;
};
