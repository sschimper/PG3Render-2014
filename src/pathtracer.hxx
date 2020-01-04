#pragma once

#include <vector>
#include <cmath>
#include <omp.h>
#include <cassert>
#include "renderer.hxx"
#include "rng.hxx"

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

			// set up variables for recursion
			Vec3f LoDirect = Vec3f(0);
			Vec3f thrput = (1.f, 1.f, 1.f);
			float pdfBrdf = 1;

			bool firstIsec = true;

			Isect prevIsect;
			Ray prevRay = ray;

			while (true)
			{
				Isect isect;
				isect.dist = 1e36f;

				// if nothing was hit by the ray, get background light information
				if (!mScene.Intersect(ray, isect))
				{
					if (mScene.GetBackground())
					{
						// float pdfLightSampling = mScene.GetBackground()->getPDF(0, ray.dir);
						float pdfLightSampling = mScene.GetBackground()->getPDF();
						float weightBRDFSampling = getBalanceHeuristic(pdfBrdf, pdfLightSampling);
						Vec3f radiance = mScene.GetBackground()->mBackgroundColor;
						LoDirect += radiance * weightBRDFSampling * thrput;
					}
					break;
				}

				// if something is hit, get the mat info from intersection point
				Vec3f normal = Normalize(isect.normal); 
				const Vec3f surfPt = ray.org + ray.dir * isect.dist;
				Frame frame;
				frame.SetFromZ(isect.normal);
				const Vec3f wog = -ray.dir;
				const Vec3f wol = frame.ToLocal(-ray.dir);
				const Material& mat = mScene.GetMaterial(isect.matID);

				// if light source is intersected, add the light to the final image
				if (isect.lightID >= 0)
				{
					// if the first ray hits the light source 
					// calculate LoDirect and return
					if (firstIsec)
					{
						const auto* firstIsectLight = mScene.GetLightPtr(isect.lightID);
						assert(firstIsectLight != 0);
						LoDirect += thrput * firstIsectLight->getRadiance();
						break;
					}

					const AbstractLight *abstLight = mScene.GetLightPtr(isect.lightID);
					float pdfLightSampling = abstLight->getPDF(isect.dist, ray.dir);
					float weightBRDFSampling;
					weightBRDFSampling = getBalanceHeuristic(pdfBrdf, pdfLightSampling); 

					float cosTheta = Dot(normal, ray.dir);
					LoDirect += ((abstLight->getRadiance() * weightBRDFSampling)) * thrput;
					break;
				}

				if (firstIsec)
				{
					firstIsec = false;
				}

				//////////////////////////////////////////////
				//			Area Light Sampling				//
				//////////////////////////////////////////////
				/*
				// initialize variables for the prob of light sampling or brdf sampling
				float lightSamplingPdfLight;
				float lightSamplingPdfBrdf;

				// ASSIGNMENT 1
				for (int i = 0; i < mScene.GetLightCount(); i++)
				{
					const AbstractLight* light = mScene.GetLightPtr(i);
					assert(light != 0);

					Vec3f wig;
					float lightDist;
					Vec3f illum = light->sampleIllumination(mRng.GetVec3f(), surfPt, frame, wig, lightDist); 

					// if the scene is a "point light scene", always do
					// light sampling
					// set the probabilities accordingly
					const PointLight* ptLight = dynamic_cast<const PointLight*>(light);
					if (ptLight != nullptr)
					{
						lightSamplingPdfLight = 1;
						lightSamplingPdfBrdf = 0;
					}
					else
					{
						lightSamplingPdfLight = light->getPDF(lightDist, wig);
						lightSamplingPdfBrdf = mat.evalBrdfPdf(wog, wig, frame.Normal());
					}

					// get the weights
					float weightLightSampling = getBalanceHeuristic(lightSamplingPdfLight, lightSamplingPdfBrdf);

					if (illum.Max() > 0)
					{
						if (!mScene.Occluded(surfPt, wig, lightDist))
						{
							LoDirect += (illum * mat.evalBrdf(frame.ToLocal(wig), wol) * weightLightSampling) * thrput;
						}
					}
				}
				*/
				//////////////////////////////////////////////
				//			Area Light Sampling	 end		//
				//////////////////////////////////////////////

				//////////////////////////////////////////////
				//				BRDF Sampling				//
				//////////////////////////////////////////////

				// initialize variables for the probability of light sampling or brdf sampling

				// set up for second ray
				Vec3f genDir; // generated direction
				Ray secondRay; // second Ray
				Isect secondRayIsect; // second intersection
				float ps; // prob of choosing the diffuse component
				float pd; // prob of choosing the specular comp.

				// generate new direction
				createSecondRay(mat, genDir, secondRay, secondRayIsect, frame, wog, surfPt, normal, pd, ps);

				// calculate pdf
				pdfBrdf = mat.evalBrdfPdf(wog, genDir, normal);

				//////////////////////////////////////////////
				//				BRDF Sampling end			//
				//////////////////////////////////////////////

				//////////////////////////////////////////////
				//				RR and Continuing			//
				//////////////////////////////////////////////

				Vec3f thrputUpdate = 1 / pdfBrdf * mat.evalBrdf(frame.ToLocal(genDir), wol) * Dot(isect.normal, genDir);
				float survivalProb = fmin(1.f, thrputUpdate.Max());

				// russian roulette
				if (mRng.GetFloat() < survivalProb)
				{
					thrput *= (thrputUpdate / survivalProb);

					prevRay = ray;
					ray.org = surfPt + genDir * EPS_RAY; // a little offset
					ray.dir = genDir;
				}
				else
				{ 
					// terminate path
					break;
				}

				//////////////////////////////////////////////
				//		RR and Continuing end				//
				//////////////////////////////////////////////

			} // end while loop

		
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

		mIterations++;
	}

	// ASSIGNMENT 2
	// select a BRDF component and create a new random direction
	void createSecondRay(const Material &mat, 
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
		pd = mat.getMaxElementInVector(mat.mDiffuseReflectance); // Exception thrown: read access violation
		ps = mat.getMaxElementInVector(mat.mPhongReflectance);
		float sumPdPs = (pd + ps);
		pd /= sumPdPs;	 // prob of choosing the diffuse component
		ps /= sumPdPs;	 // prob of choosing the specular comp.

		float r1 = mRng.GetFloat();
		float r2 = mRng.GetFloat();

		if (mRng.GetFloat() <= pd) 
		{
			genDir = frame.ToWorld(mat.sampleDiffuse(r1, r2));
		}
		else 
		{
			genDir = mat.sampleGlossy(wog, normal, r1, r2);
		}
	}
	
	// get balance heuristic
	float getBalanceHeuristic(float fPdf, float gPdf)
	{
		return (fPdf) / (fPdf + gPdf);
	}

	Rng mRng;
};
