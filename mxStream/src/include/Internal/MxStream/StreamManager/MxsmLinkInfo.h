/*
* -------------------------------------------------------------------------
*  This file is part of the Vision SDK project.
* Copyright (c) 2025 Huawei Technologies Co.,Ltd.
*
* Vision SDK is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*
*           http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
* -------------------------------------------------------------------------
 * Description: Streaming status and variable definition.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXSM_LINK_INFO_H
#define MXSM_LINK_INFO_H

enum MxsmLinkState {
    MXSM_LINK_STATE_NORMAL = 0,
    MXSM_LINK_STATE_LINKPENDING,    // wait for linking
    MXSM_LINK_STATE_RELINKPENDING,  // wait for relinking
    MXSM_LINK_STATE_DELETEPENDING,  // wart for breaking link
    MXSM_LINK_STATE_NORMALPENDING,
    MXSM_LINK_STATE_BUTT
};

enum MxsmLinkInfoFlags { ES_LINK_INFO_FLAGS_SEARCHED = 0, ES_LINK_INFO_FLAGS_BUTT };

class MxsmLinkInfo {
public:
    MxsmLinkInfo(MxsmLinkState initState)
    {
        state = initState;
        flags = 0;
    }

    ~MxsmLinkInfo() {}

    MxsmLinkInfo()
    {
        state = MXSM_LINK_STATE_NORMAL;
        flags = 0;
    }

    MxsmLinkState GetState()
    {
        return state;
    }

    void SetState(MxsmLinkState targetState)
    {
        state = targetState;
    }

    void ClearFlag()
    {
        flags = 0;
    }

    void SetFlag(MxsmLinkInfoFlags flag)
    {
        flags |= (uint32_t)flag;
    }

    bool HaveFlag(MxsmLinkInfoFlags flag)
    {
        return flags & ((uint32_t)flag);
    }

private:
    MxsmLinkState state;
    uint32_t flags;
};

#endif  // MXSM_LINK_INFO_H