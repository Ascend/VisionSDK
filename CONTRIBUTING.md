# 为 Vision SDK 贡献

感谢您考虑为 Vision SDK 做出贡献！我们欢迎任何形式的贡献，包括错误修复、功能增强、文档改进等，甚至只是反馈。无论您是经验丰富的开发者还是第一次参与开源项目，您的帮助都是非常宝贵的。

您可以通过多种方式支持本项目：

* 通过 [VisionSDK 新手任务池](https://gitcode.com/Ascend/VisionSDK/issues/13) 参与贡献。
* 完成社区任务，领取奖励：[昇腾开源软件社区任务](https://www.hiascend.com/developer/activities/details/5dbf59b2dce14f91afb157f5a52f332d#tab0)。
* 审查 Pull Request 并协助其他贡献者。
* 传播项目：在博客文章、社交媒体上分享 Vision SDK，或给仓库点个 ⭐。

参与贡献前，请先签署开放项目贡献者许可协议（[CLA](https://clasign.osinfra.cn/sign/gitee_ascend-1611222220829317930)），提前了解[《社区行为规范》](https://gitcode.com/Ascend/community/blob/master/docs/contributor/code-of-conduct.md)。

## 贡献方式

### Pull Request

提交 PR 前，请先了解 [《Pull Request (PR) 提交流程指南》](https://gitcode.com/Ascend/community/blob/master/docs/contributor/pr-guide.md) 和 [PR 最佳实践](#pr-最佳实践)，掌握从 Fork 到提交、从代码审查到合并的完整 PR 流程，包括 CI 检查、标签要求和合并规范。

### Issue

提交 Issue 前，请先了解 [《Issue 创建与处理指南》](https://gitcode.com/Ascend/community/blob/master/docs/contributor/issue-guide.md)，学习如何有效创建、分类和管理 Issue，包括问题报告、功能请求的规范格式。

### SIG 会议

如果涉及 patch、头文件宏、API 接口等更新，需提交社区在 SIG 例会进行评审，社区定期例会与活动参见[会议日历](https://meeting.ascend.osinfra.cn/?sig=sig-MindSeriesSDK)。

## PR 最佳实践

1. **Fork 仓库**：在 GitCode 平台代码仓库右上角点击 "Fork" 按钮，Fork 一份源代码到个人仓。
2. **克隆到本地**：
   将 Fork 到个人仓的代码克隆到本地进行代码开发。

   ```bash
   git clone https://gitcode.com/<your-username>/VisionSDK.git
   cd VisionSDK
   ```

3. **创建开发分支**：

   ```bash
   git checkout -b {new_branch_name} origin/master
   ```

4. **代码开发**：
   质量符合[开发规范](#参考)和[安全编程指导](#参考)。
5. **开发构建验证**：
   1. 拉取镜像环境。以 910B 场景为例，拉取镜像：

      ```bash
      docker pull swr.cn-south-1.myhuaweicloud.com/ascendhub/vision-sdk:{version}-ubuntu22.04-py3.12
      ```

   2. 进入 `build` 子目录，执行构建脚本：

      ```bash
      cd build
      bash build.sh
      ```

   3. 在提交代码前，请补充测试用例并确保所有测试通过，本地执行 UT：

      ```bash
      cd build
      bash build.sh ut
      ```

6. **执行 pre-commit 检查**：
   本地提交代码前请先执行 pre-commit 检查，检查指导参见 [pre-commit 本地运行指南](https://gitcode.com/Ascend/community/blob/master/docs/contributor/pre-commit-guide.md)。
7. **提交 Pull Request**：
   * 保持 PR 小规模，一次 PR 只解决一个问题，单个 PR 不超过 1000 行（含测试）代码变更。
   * 及时更新，定期同步上游主分支，及时响应评审意见。
   * 描述清晰，详细描述变更原因和方式，提供测试方法，添加截图或示例。
8. **社区评审**：
   如果涉及 patch、头文件宏、API 接口等更新，需提交社区在 SIG 例会进行评审，社区定期例会与活动参见[会议日历](https://meeting.ascend.osinfra.cn/?sig=sig-MindSeriesSDK)。

## 分支/Tag 命名规则

自研代码仓

| 分支类型 | 分支名规则 | 示例 | 说明 | tag 名规则 | tag 示例 |
| ------- | --------- | --- | --- | --------- | ------- |
| 主干&开发 | master | - | 主干开发分支 | - | - |
| release | release/v<版本号> | release/v26.1.0 | 正式版本开发分支 | v<版本号>[-beta.<序号>] | v26.1.0、v26.1.0-beta.1 |
| poc | poc/<基线分支>/<描述> | poc/release-v26.1.0/auth-redesign | 后续合入主干 | poc/<基线分支>/<描述>-v<序号> | poc/release-v26.1.0/auth-redesign-v1 |

Fork 开源社区代码仓

| 分支类型 | 分支名规则 | 示例 | 说明 | tag 名规则 | tag 示例 |
| ------- | --------- | --- | --- | --------- | ------- |
| 社区分支 | - | master | 不合入代码 | - | - |
| release | release/<社区分支>-<产品版本号> | release/master-26.1.0 | 正式版本开发分支 | v<产品版本号>-<社区分支> | v26.1.0-master |
| poc | poc/<基线分支>/<描述> | poc/release-v26.1.0/auth-redesign | 后续合入 release 分支 | poc/<基线分支>/<描述>-v<序号> | poc/release-v26.1.0/auth-redesign-v1 |

## 参考

* 开发规范
  * [《Ascend C++ 编码风格指南》](https://gitcode.com/Ascend/community/blob/master/docs/contributor/Ascend-cpp-coding-style-guide.md)
  * [《Ascend Python 编码风格指南》](https://gitcode.com/Ascend/community/blob/master/docs/contributor/Ascend-python-coding-style-guide.md)
* 安全编程指导
  * [《Ascend C++ 安全编程指南》](https://gitcode.com/Ascend/community/blob/master/docs/contributor/Ascend-cpp-secure-coding-guide.md)
  * [《Ascend Python 安全编程指南》](https://gitcode.com/Ascend/community/blob/master/docs/contributor/Ascend-python-secure-coding-guide.md)
* [《Ascend 安全编译选项指南(C&C++)》](https://gitcode.com/Ascend/community/blob/master/docs/contributor/Ascend-secure-compile-guide.md)
* 更多社区相关规范，请访问 [Ascend 社区 community](https://gitcode.com/Ascend/community)
