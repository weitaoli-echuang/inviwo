
#ifndef COOK_TORRANCE_BRDF_CL
#define COOK_TORRANCE_BRDF_CL

#include "shading/shadingmath.cl"
#include "shading/microfacet.cl"

float CookTorranceBRDF(const float3 wo, const float3 wi, const float f0, const float m) {
    if(!sameHemisphere(wo, wi)) {
        return 0.f;
    }
    Microfacet distribution;
    float3 wh = normalize(wo+wi);
    float NdotWh = absCosTheta(wh);
    distribution.D = BeckmannDistribution(NdotWh, m);
    //distribution.D = BlinnPhongDistribution(NdotWh, m);
    distribution.G = CookTorranceGeometry(wo, wi, wh);
    distribution.F = fresnelSchlick(NdotWh, f0); 
    return microfacetBRDF(wo, wi, wh, &distribution);
}
float3 CookTorranceSample(const float3 wo, float m, const float2 rnd) {
    return BeckmannSample(wo, m, rnd);    
}
float CookTorrancePdf(const float3 wo, const float3 wi, const float3 N, float m) {
    if (sameHemisphereN(wo, wi, N)) {
        return BeckmannPdf(wo, wi, m);
    } else {
        return 0.f;
    }
}

#endif
