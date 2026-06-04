# 贡献指南

感谢您考虑为 Vision SDK 做出贡献！我们欢迎任何形式的贡献，包括错误修复、功能增强、文档改进等，甚至只是反馈。无论您是经验丰富的开发者还是第一次参与开源项目，您的帮助都是非常宝贵的。

- 通过[Issues](https://gitcode.com/Ascend/VisionSDK/issues)反馈问题。
- 建议或实现新功能。
- 改进或扩展文档。
- 审查Pull Request并协助其他贡献者。
- 传播项目：在博客文章、社交媒体上分享VisionSDK，或给仓库点个⭐。
- 完成[社区任务](https://gitcode.com/Ascend/VisionSDK/issues/13)，领取[任务奖励](https://www.hiascend.com/developer/activities/details/5dbf59b2dce14f91afb157f5a52f332d#tab0)。


请先提前了解社区相关规范：

- [Ascend 开源项目行为守则](https://gitcode.com/Ascend/community/blob/master/docs/contributor/code-of-conduct.md)
- [Issue 提交指南](https://gitcode.com/Ascend/community/blob/master/docs/contributor/issue-guide.md)
- [社区 Issue 处理流程指导](https://gitcode.com/Ascend/community/blob/master/docs/contributor/issue-workflow-guidelines.md)
- [Ascend 社区开发者测试贡献指南](https://gitcode.com/Ascend/community/blob/master/docs/contributor/developer-testing-guide.md)
- [Ascend 开源与第三方软件建仓及分支命名指导](https://gitcode.com/Ascend/community/blob/master/docs/contributor/third-party-repo-branch-guide.md)
- [Ascend 开源与第三方软件管理规范](https://gitcode.com/Ascend/community/blob/master/docs/contributor/third-party-software-management-guide.md)
- [社区安全设计规范](https://gitcode.com/Ascend/community/blob/master/docs/contributor/security-design-guideline.md)
- [C++代码编程规范](https://gitcode.com/Ascend/community/blob/master/docs/contributor/Ascend-cpp-coding-style-guide.md)
- [C++代码安全规范](https://gitcode.com/Ascend/community/blob/master/docs/contributor/Ascend-cpp-secure-coding-guide.md)
- [Python代码编程规范](https://gitcode.com/Ascend/community/blob/master/docs/contributor/Ascend-python-coding-style-guide.md)
- [Python代码安全规范](https://gitcode.com/Ascend/community/blob/master/docs/contributor/Ascend-python-secure-coding-guide.md)
- [PR 提交指南](https://gitcode.com/Ascend/community/blob/master/docs/contributor/pr-guide.md)

### 开发与测试

1. **Fork仓库**

   在GitCode 平台点击仓库右上角 "Fork" 按钮，将仓库克隆到个人账户。

2. **克隆到本地**：

   ```bash
   git clone https://gitcode.com/<your-username>/VisionSDK.git
   cd VisionSDK
   ```

3. **创建开发者分支**：

   ```
   git checkout -b {new_branch_name} origin/master
   ```

4. **代码开发**

   请遵循[C++代码编程规范](https://gitcode.com/Ascend/community/blob/master/docs/contributor/Ascend-cpp-coding-style-guide.md)，[C++代码安全规范](https://gitcode.com/Ascend/community/blob/master/docs/contributor/Ascend-cpp-secure-coding-guide.md)，[Python代码编程规范](https://gitcode.com/Ascend/community/blob/master/docs/contributor/Ascend-python-coding-style-guide.md)和[Python代码安全规范](https://gitcode.com/Ascend/community/blob/master/docs/contributor/Ascend-python-secure-coding-guide.md)。

5. **开发构建验证**：

   1. 从[VisionSDK镜像](https://www.hiascend.com/developer/ascendhub/detail/9e0edaf9488b447b951072c5c61ce8f1)下载 VisionSDK 镜像，运行容器。
   2. 参考[构建](./docs/zh/installation_guide.md#自行构建)进行构建或测试。

6. **本地提交**：

   本地提交代码前请先执行pre-commit检查，检查指导参见[pre-commit本地运行指南](https://gitcode.com/Ascend/community/blob/master/docs/contributor/pre-commit-guide.md)。

7. **提交 Pull Request**

   请先完成[CLA签署](https://gitcode.com/Ascend/community/blob/master/docs/contributor/developer-testing-guide.md)，然后提交 PR 并等待代码审查。

8. **社区评审**

   如果涉及 patch、头文件宏、API 接口等更新，需提交社区评审。
