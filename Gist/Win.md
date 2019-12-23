# 解决VM 与 Device/Credential Guard 不兼容。在禁用 Device/Credential Guard 后，可以运行 VM 的方法

在启用了Credential Guard或Device Guard的Windows 10主机上启动12.5版之前的VMware Workstation中的虚拟机时，将显示蓝色诊断屏幕（BSOD）。

会看到类似于以下内容的错误：

VMware Workstation和Device / Credential Guard不兼容。禁用Device / Credential Guard后，可以运行VMware Workstation。

原因：
1、出现此问题的原因是Device Guard或Credential Guard与Workstation不兼容。
2、Windows系统的Hyper-V不兼容导致。

解决方法：

## 步骤一：禁用 Device Guard 或 Credential Guard：

禁用用于启用Credential Guard的组策略设置。
在主机操作系统上，右键单击“开始” > “运行”，键入gpedit.msc，然后单击“ 确定”。本地组策略编辑器打开。
转至本地计算机策略 > 计算机配置 > 管理模板>系统 >Device Guard（或者是： 设备防护） > 启用基于虚拟化的安全性。
选择已禁用。

转到“ 控制面板” >“ 卸载程序” >“ 打开或关闭Windows功能”以关闭Hyper-V。
选择不重启。

## 步骤二：通过命令关闭Hyper-V（控制面板关闭Hyper-V起不到决定性作用，要彻底关闭Hyper-V） 

        以管理员身份运行Windows Powershell (管理员)（Windows键+X）
    
        运行下面命令并重启电脑：

- bcdedit /set hypervisorlaunchtype off
  