                     深圳市卡联科技有限公司--手持机硬件测试程序包（MiniGui版）
1.将maintest和res文件夹放在POS机的一个“/opt/”目录内，运行“./maintest”即可
  软件操作步骤：见主目录下“Linux二次开发操作指南（手持机）”
2.源代码
     |
     |
     res //程序所需要的图片
     |
     |
     maintest //主函数
     |
     |
     gprs //GPRS测试函数
     |
     |
     listchina //RTC测试函数
     |
     |
     Makefile //make命令后，生成可执行文件：maintest 
     |
     |
     mkfile //shell文件，可直接执行，将编译好的可执行文件：maintest放入“/nfs/”内，     