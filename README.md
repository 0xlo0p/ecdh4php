
# ecdh_for_php 介绍
这是一个用C实现ECDH的php扩展，实例中使用NID_secp224r1(713)作为曲线参数
这个扩展是为了研究微信mmtls协议一部分而写的，所以主要功能也是适应微信对应的规则而写


# ecdh 科普
![ecdh](http://mmbiz.qpic.cn/mmbiz/csvJ6rH9MctDOPR1TbFBSWx5aBUCibJsqbA48k433vyHrlibuG3ZjSLdibE6dj9hT50rlYicib8Dg9Aa2q3v9vLYU5A/0?wx_fmt=jpeg)

* 首先看一个，会遭受到攻击的密钥协商过程。通信双方Alice和Bob使用ECDH密钥交换协议进行密钥协商，ECDH密钥交换协议拥有两个算法：
* 密钥生成算法ECDH_Generate_Key，输出一个公钥和私钥对(ECDH_pub_key, ECDH_pri_key)，ECDH_pri_key需要秘密地保存，ECDH_pub_key可以公开发送给对方。
* 密钥协商算法ECDH_compute_key，以对方的公钥和自己的私钥作为输入，计算出一个密钥Key，ECDH_compute_key算法使得通信双方计算出的密钥Key是一致的。
* 这样一来Alice和Bob仅仅通过交换自己的公钥ECDH_pub_key，就可以在Internet这种公开信道上共享一个相同密钥Key，然后用这个Key作为对称加密算法的密钥，进行加密通信。 
* 而微信则是拿这个Key作为Aes密钥进行加解密通信数据


# php使用方法以及结果
```php
<?php
$serv_pubKey="0445800B423268F081731AE1A873E5342427E8DDCD768C017D49A6BA340217FD12976EBD5D0A332E8D6FA1C45AF92A1274411DE10B950B77DC";

var_dump(compute_key_auto($serv_pubKey));
?>
```
```php
array(2) {
  ["cpubkey"]=>
  string(114) "042c21104e900e2f27f1aef44a7a63182f4dc9d8c922cb1a768d626cedcb1d135bb9fd08e0cf9b28a8990cb0f45899178a571c23ce15474421"
  ["sharekey"]=>
  string(32) "8e7bd77eede3ae6af55e9a0217b245ba"
}
cpubkey为客户的公钥
sharekey为共享的密钥，密钥交换的结果
```
