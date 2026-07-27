/*
* Copyright (c) <2023> Side Effects Software Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. The name of Side Effects Software may not be used to endorse or
*    promote products derived from this software without specific prior
*    written permission.
*
* THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
* NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "HoudiniApi.h"
#include "HoudiniEngineUtility.h"

#include <iostream>
#include <string>


std::string 
HoudiniEngineUtility::getLastError(HAPI_Session* session)
{
    int buffer_length = 0;
    if(HoudiniApi::GetStatusStringBufLength(
        session,
        HAPI_STATUS_CALL_RESULT,
        HAPI_STATUSVERBOSITY_ERRORS,
        &buffer_length) != HAPI_RESULT_SUCCESS)
        return {};

    if (buffer_length <= 0)
        return {};

    std::vector<char> buffer(buffer_length);
    if(HoudiniApi::GetStatusString(session, HAPI_STATUS_CALL_RESULT, buffer.data(), buffer_length) != HAPI_RESULT_SUCCESS)
        return {};
    return std::string(buffer.data());
}

std::string 
HoudiniEngineUtility::getLastCookError(HAPI_Session* session)
{
    int buffer_length = 0;
    if(HoudiniApi::GetStatusStringBufLength(
        session,
        HAPI_STATUS_COOK_RESULT,
        HAPI_STATUSVERBOSITY_ERRORS,
        &buffer_length) != HAPI_RESULT_SUCCESS)
        return {};

    if (buffer_length <= 0)
        return {};

    std::vector<char> buffer(buffer_length);
    if(HoudiniApi::GetStatusString(session, HAPI_STATUS_COOK_RESULT, buffer.data(), buffer_length) != HAPI_RESULT_SUCCESS)
        return {};
    return std::string(buffer.data());
}


std::string
HoudiniEngineUtility::getConnectionError()
{
    int buffer_length = 0;
    if(HoudiniApi::GetConnectionErrorLength(&buffer_length) != HAPI_RESULT_SUCCESS)
        return {};

    if (buffer_length <= 0)
        return {};

    std::vector<char> buffer(buffer_length);
    if(HoudiniApi::GetConnectionError(buffer.data(), buffer_length, true) != HAPI_RESULT_SUCCESS)
        return {};
    return std::string(buffer.data());
}

std::string 
HoudiniEngineUtility::getString(const HAPI_Session * session, HAPI_StringHandle string_handle)
{
    int length = 0;
    if(HoudiniApi::GetStringBufLength(session, string_handle, &length) != HAPI_RESULT_SUCCESS)
        return {};

    std::vector<char> buffer(length + 1);
    if(HoudiniApi::GetString(session, string_handle, buffer.data(), length) != HAPI_RESULT_SUCCESS)
        return {};
    return std::string(buffer.data());
}


bool
HoudiniEngineUtility::saveToHip(const HAPI_Session * session, const std::string& filename)
{
    HAPI_Result result = HoudiniApi::SaveHIPFile(session, filename.c_str(), /*lock_nodes=*/false);
    return result == HAPI_RESULT_SUCCESS;
}