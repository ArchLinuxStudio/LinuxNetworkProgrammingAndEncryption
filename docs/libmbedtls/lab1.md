# Lab1

通过本章的学习，你已经有能力书写密码学方面的代码。配合之前学过的网络编程知识，这一节需要你完成一个实验。内容是在客户端与服务端之间实现一个加密通讯，你可以直接在 Lab0 的基础上进行改进。

具体要求是，对客户端与服务端之间的通讯进行 AEAD 加密，如使用 AES GCM。需要格外注意的是，在你处理密文数据时，要注意他们的长度。此外，服务器与客户端进行通讯时，以什么样的格式传递数据也是你需要考虑的问题。

下面提供答案的样例程序，注意，在你没有自行实现之前不要查看答案。

https://github.com/ArchLinuxStudio/ShadowButterfly/tree/master/src/1-basic_encrypt
