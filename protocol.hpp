//
// stock.hpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef SERIALIZATION_STOCK_HPP
#define SERIALIZATION_STOCK_HPP

#include <string>

namespace wayixia { namespace protocol {

#define PACKAGE_HEADER			0xFF		// 0xFF
#define PACKAGE_TAIL			0x00		// 0x00
#define PACKAGE_SIGNDATA		0x2d0x2d0x2d0x710x730x2d0x2d0x2d	// ---qs---
#define PACKAGE_SIGNDATA_LEN	8


// 数据包定义
typedef 
enum package_type_ 
{
	basic	=	0x0000,		// 基本包类型
	heart	=	0x0001,		// 心跳包
	rpc		=	0x0002,		// RPC 包
	forward	=	0x0004		// 转发包
} package_type;

typedef 
struct package_ 
{
	uint8  sign[PACKAGE_SIGNDATA_LEN];	//包头标示
	uint32 version;						// 版本协议
	uint32 type;						// 数据包包类型
	uint32 size;						// 数据块大小
} package;

typedef 
struct package_forward_ 
{
	

}

/// Structure to hold information about a single stock.
struct stock
{
  std::string code;
  std::string name;
  double open_price;
  double high_price;
  double low_price;
  double last_price;
  double buy_price;
  int buy_quantity;
  double sell_price;
  int sell_quantity;

  template <typename Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & code;
    ar & name;
    ar & open_price;
    ar & high_price;
    ar & low_price;
    ar & last_price;
    ar & buy_price;
    ar & buy_quantity;
    ar & sell_price;
    ar & sell_quantity;
  }
};

} }// namespace wayixia

#endif // SERIALIZATION_STOCK_HPP