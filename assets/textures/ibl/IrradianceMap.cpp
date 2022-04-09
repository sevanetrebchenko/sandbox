// -----------------------------------------------------------------------------
// @Project Name    SwitchBlade
// @filename        IrradianceMap.cpp
// @date            3/21/2022
// @author          Jordan Hoffmann
//
// Copyright � 2021 DigiPen, All rights reserved.
// -----------------------------------------------------------------------------
#include <iostream>
#include <map>
#include <array>
#include "IrradianceMap.h"
#include "rgbe.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include <thread>

namespace
{
    // Read an HDR image in .hdr (RGBE) format.
    void read(const std::filesystem::path &file, std::vector<float> &image,
        int &width, int &height)
    {
        rgbe_header_info info;
        char errbuf[100] = {0};

        // Open file and read width and height from the header
        FILE *fp = fopen(file.string().c_str(), "rb");
        if (!fp)
        {
            printf("Can't open file: %ws\n", file.c_str());
            exit(-1);
        }
        int rc = RGBE_ReadHeader(fp, &width, &height, &info, errbuf);
        if (rc != RGBE_RETURN_SUCCESS)
        {
            printf("RGBE read error: %s\n", errbuf);
            exit(-1);
        }

        // Allocate enough memory
        image.resize(3 * width * height);

        // Read the pixel data and close the file
        rc = RGBE_ReadPixels_RLE(fp, &image[0], width, height, errbuf);
        if (rc != RGBE_RETURN_SUCCESS)
        {
            printf("RGBE read error: %s\n", errbuf);
            exit(-1);
        }
        fclose(fp);

        printf("Read %ws (%dX%d)\n", file.c_str(), width, height);
    }

// Write an HDR image in .hdr (RGBE) format.
    void write(const std::filesystem::path &file, std::vector<float> &image,
        const int width, const int height)
    {
        rgbe_header_info info;
        char errbuf[100] = {0};

        // Open file and write width and height to the header
        FILE *fp = fopen(file.string().c_str(), "wb");
        int rc = RGBE_WriteHeader(fp, width, height, NULL, errbuf);
        if (rc != RGBE_RETURN_SUCCESS)
        {
            printf("RGBE write error: %s\n", errbuf);
            exit(-1);
        }

        // Write the pixel data and close the file
        rc = RGBE_WritePixels_RLE(fp, &image[0], width, height, errbuf);
        if (rc != RGBE_RETURN_SUCCESS)
        {
            printf("RGBE write error: %s\n", errbuf);
            exit(-1);
        }
        fclose(fp);

        printf("Wrote %ws (%dX%d)\n", file.c_str(), width, height);
    }

    const std::array<std::vector<float (*)(float, float, float)>, 3> Y = {{
        {
            +[](float x, float y, float z) -> float
            {
                return 0.5f * glm::sqrt(1.f / glm::pi<float>());
            }
        },
        {
            +[](float x, float y, float z) -> float
            {
                return 0.5f * glm::sqrt(3.f / glm::pi<float>()) * y;
            },
            +[](float x, float y, float z) -> float
            {
                return 0.5f * glm::sqrt(3.f / glm::pi<float>()) * z;
            },
            +[](float x, float y, float z) -> float
            {
                return 0.5f * glm::sqrt(3.f / glm::pi<float>()) * x;
            }
        },
        {
            +[](float x, float y, float z) -> float
            {
                return 0.5f * glm::sqrt(15.f / glm::pi<float>()) * x * y;
            },
            +[](float x, float y, float z) -> float
            {
                return 0.5f * glm::sqrt(15.f / glm::pi<float>()) * y * z;
            },
            +[](float x, float y, float z) -> float
            {
                return 0.25f * glm::sqrt(5.f / glm::pi<float>()) * ((3.f * z * z) - 1.f);
            },
            +[](float x, float y, float z) -> float
            {
                return 0.5f * glm::sqrt(15.f / glm::pi<float>()) * x * z;
            },
            +[](float x, float y, float z) -> float
            {
                return 0.25f * glm::sqrt(15.f / glm::pi<float>()) * ((x * x) - (y * y));
            }
        }
    }};

    constexpr std::array<float, 3> A = {{
        glm::pi<float>(),
        (2.f / 3.f) * glm::pi<float>(),
        0.25f * glm::pi<float>()
    }};

};

Texture IrradianceMap::Generate(const std::filesystem::path &inFile)
{
    // Read in-file name from command line, create out-file name
    std::filesystem::path outFile = inFile;
    outFile.replace_filename(
        inFile.stem().string()
            + ".irr"
            + inFile.extension().string());

    std::cout << "Generating " << outFile << ". This may take a while" << std::endl;

    int inWidth, inHeight;
    std::vector<float> inImage;
    read(inFile, inImage, inWidth, inHeight);

    int outWidth = inWidth/2, outHeight = inHeight/2;
    std::vector<float> outImage(3 * outWidth * outHeight);

    std::vector<std::thread> workers;
    workers.reserve(std::max(9, outHeight));

    // Project input image onto spherical harmonic coefficients
    std::array<std::vector<glm::vec3>, 3> L;
    for (size_t l = 0; l < 3; ++l)
    {
        L[l].resize(Y[l].size());
        for (size_t m = 0; m < Y[l].size(); ++m)
        {
            workers.emplace_back([inWidth, inHeight, l, m, &L, &inImage]()
            {
                for (int jIn = 0; jIn < inHeight; ++jIn)
                {
                    for (int iIn = 0; iIn < inWidth; ++iIn)
                    {
                        const size_t lIndex = (inWidth * jIn + iIn) * 3u;
                        const glm::vec3 &Light = *reinterpret_cast<glm::vec3 *>(inImage.data() + lIndex);

                        const float dThetaIn = glm::pi<float>() / (float) inHeight;
                        const float dPhiIn = glm::pi<float>() / (float) inWidth;

                        const float thetaIn = ((float) jIn + 0.5f) * dThetaIn;
                        const float phiIn = 2 * ((float) iIn + 0.5f) * dPhiIn;

                        const float xIn = glm::sin(thetaIn) * glm::cos(phiIn);
                        const float yIn = glm::sin(thetaIn) * glm::sin(phiIn);
                        const float zIn = glm::cos(thetaIn);

                        L[l][m] += Light * Y[l][m](xIn, yIn, zIn) * glm::sin(thetaIn) * dThetaIn * dPhiIn;
                    }
                }
            });
        }
    }

    for (std::thread &worker: workers)
        worker.join();
    workers.clear();

    // evaluate irradiance at every direction
    for (int jOut = 0; jOut < outHeight; jOut++)
    {
        workers.emplace_back([L, jOut, outWidth, outHeight, &outImage]()
        {
            for (int iOut = 0; iOut < outWidth; iOut++)
            {
                const size_t irrOutIndex = (outWidth * jOut + iOut) * 3u;
                glm::vec3 &irrOut = *reinterpret_cast<glm::vec3 *>(outImage.data() + irrOutIndex);
                irrOut = {0.f, 0.f, 0.f};

                for (size_t l = 0; l < 3; ++l)
                {
                    const float A_l = A[l];
                    for (size_t m = 0; m < Y[l].size(); ++m)
                    {
                        const float dThetaOut = glm::pi<float>() / (float) outHeight;
                        const float dPhiOut = glm::pi<float>() / (float) outWidth;

                        const float thetaOut = ((float) jOut + 0.5f) * dThetaOut;
                        const float phiOut = 2 * ((float) iOut + 0.5f) * dPhiOut;

                        const float xOut = glm::sin(thetaOut) * glm::cos(phiOut);
                        const float yOut = glm::sin(thetaOut) * glm::sin(phiOut);
                        const float zOut = glm::cos(thetaOut);

                        const glm::vec3 E_lm = A_l * L[l][m];
                        irrOut += E_lm * Y[l][m](xOut, yOut, zOut);
                    }
                }
            }
        });
    }

    for (std::thread &worker: workers)
        worker.join();
    workers.clear();


    // Write the output image
    write(outFile, outImage, outWidth, outHeight);
    return {outFile};
}
