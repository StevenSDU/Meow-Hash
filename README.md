# Meow-Hash
任务九：Meow Hash

1、任务截图：

![image](https://user-images.githubusercontent.com/108848022/181679213-e62138ea-e453-47de-aeb7-7c9893d659bf.png)

2、代码备注：

（1）本次实验需要用到有关Meow Hash的相关内容，相关函数在PPT中有简略介绍，在此不再赘述。

（2）meow hash作为一种快速哈希方法，对于1024bit的密钥，进行128bit拆分，利用SIMD指令进行加速运算。

![image](https://user-images.githubusercontent.com/108848022/181679354-9716379d-d1ae-4f23-9824-0c05111c685c.png)

（3）meow hash函数的特性：可逆性。也就是说可以通过任意想构造的哈希值逆推出我们需要使用的密钥。

![image](https://user-images.githubusercontent.com/108848022/181679557-bf1336a5-182d-4eaa-bff3-fb49f713f613.png)

（4）最终我们将输入信息Wei_Xiaoyang_202000460027和输出哈希值sdu_cst_20220610写入函数中，通过可逆性求出符合题意的密钥，如下图所示：

![image](https://user-images.githubusercontent.com/108848022/181680255-29111d95-75e6-4ec6-96a2-c0429c6c6dd9.png)
