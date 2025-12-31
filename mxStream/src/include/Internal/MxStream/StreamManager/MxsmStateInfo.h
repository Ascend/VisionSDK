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
 * Description: Definition of the status of the plug-in connection.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXSM_STATE_INFO_H
#define MXSM_STATE_INFO_H

enum MxsmPendingState { MXSM_PENDING_NONE = 0, MXSM_PENDING_FORMER, MXSM_PENDING_NEXT, MXSM_PENDING_BUTT };

enum MxsmElementState {
    MXSM_ELEMENT_STATE_NORMAL = 0,
    MXSM_ELEMENT_STATE_NEW,          // marked as new element
    MXSM_ELEMENT_STATE_DELETE,       // marked as need to be deleted
    MXSM_ELEMENT_STATE_LINKPENDING,  // marked as pending link
    MXSM_ELEMENT_STATE_BUTT
};

enum MxsmElementStateInfoFlags {
    MXSM_STATE_INFO_FLAGS_GRAPHAGENTPROC = 0,
    MXSM_STATE_INFO_FLAGS_PROPSPROC,
    MXSM_STATE_INFO_FLAGS_GROUPED,
    MXSM_STATE_INFO_FLAGS_BUTT
};

class MxsmStateInfo {
public:
    MxsmStateInfo()
    {
        elementState_ = MXSM_ELEMENT_STATE_BUTT;
        pendingState_ = MXSM_PENDING_NONE;
        flags_ = 0;
    }

    ~MxsmStateInfo() {}

    MxsmElementState GetState()
    {
        return elementState_;
    }

    MxsmPendingState GetPending()
    {
        return pendingState_;
    }

    uint32_t GetFlags()
    {
        return flags_;
    }

    void SetStateNew()
    {
        elementState_ = MXSM_ELEMENT_STATE_NEW;
        pendingState_ = MXSM_PENDING_NONE;
    }

    void SetStateNormal()
    {
        elementState_ = MXSM_ELEMENT_STATE_NORMAL;
        pendingState_ = MXSM_PENDING_NONE;
    }

    void SetStateLinkPending(MxsmPendingState pending)
    {
        elementState_ = MXSM_ELEMENT_STATE_LINKPENDING;
        pendingState_ = (MxsmPendingState)(pendingState_ | pending);
    }

    void SetStateDelete()
    {
        elementState_ = MXSM_ELEMENT_STATE_DELETE;
        pendingState_ = MXSM_PENDING_NONE;
    }

    bool IsStateNew()
    {
        return elementState_ == MXSM_ELEMENT_STATE_NEW;
    }

    bool IsStateNormal()
    {
        return elementState_ == MXSM_ELEMENT_STATE_NORMAL;
    }

    bool IsStateLinkPending()
    {
        return elementState_ == MXSM_ELEMENT_STATE_LINKPENDING;
    }

    bool IsFormerPending()
    {
        return pendingState_ & MXSM_PENDING_FORMER;
    }

    bool IsNextPending()
    {
        return pendingState_ & MXSM_PENDING_NEXT;
    }

    bool IsBothPending()
    {
        return (pendingState_ & MXSM_PENDING_FORMER) && (pendingState_ & MXSM_PENDING_NEXT);
    }

    bool IsStateDelete()
    {
        return elementState_ == MXSM_ELEMENT_STATE_DELETE;
    }

    void UnsetFlag(MxsmElementStateInfoFlags flag)
    {
        flags_ &= (~(uint32_t)flag);
    }

    void SetFlag(MxsmElementStateInfoFlags flag)
    {
        flags_ |= (uint32_t)flag;
    }

    bool HaveFlag(MxsmElementStateInfoFlags flag)
    {
        return flags_ & ((uint32_t)flag);
    }

    void ClearFlags()
    {
        flags_ = 0;
    }

    void Clear()
    {
        SetStateNormal();
        ClearFlags();
    }

private:
    MxsmElementState elementState_; /* 插件的状态，正常情况下是Normal,动态编排时会根据新Stream改变 */
    MxsmPendingState pendingState_; /* 插件前后连接pending状态，插件状态mState为MXSM_ELEMENT_STATE_LINKPENDING有效 */
    uint32_t flags_;
};

#endif  // MXSM_STATE_INFO_H