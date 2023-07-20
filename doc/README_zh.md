中文 | [English](../README.md)
<!-- TOC -->
* [elog4cpp](#elog4cpp)
  * [快速开始](#快速开始)
    * [要求](#要求)
    * [安装与引入](#安装与引入)
    * [开始使用](#开始使用)
  * [如何配置](#如何配置)
    * [全局配置](#全局配置)
      * [配置方式](#配置方式)
      * [使用示例](#使用示例)
    * [局部配置](#局部配置)
  * [详细接口描述](#详细接口描述)
    * [Formatter](#formatter)
      * [内置formatter](#内置formatter)
      * [自定义formatter](#自定义formatter)
    * [Micros](#micros)
      * [ENABLE_ELG_LOG](#enableelglog)
      * [ENABLE_ELG_CHECK](#enableelgcheck)
    * [其他杂项](#其他杂项)
<!-- TOC -->

# elog4cpp
[![License](https://img.shields.io/badge/License-MIT-green)](https://github.com/ACking-you/elog4cpp/blob/master/LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Cross--platformable-blue)](https://img.shields.io/badge/Platform-Cross--platformable-blue)
[![Language](https://img.shields.io/badge/Language-C%2B%2B11%20or%20above-red)](https://en.cppreference.com/w/cpp/compiler_support/11)

`elog4cpp` ：意味着这是一个使用上非常 `easy`，同时性能上也非常 `efficiency` c++ log 日志库。 支持c++11及以上，并且完全的跨平台。

使用 `easy` 体现在：

* api简单，你只需要关注一个 `elog::Log` 类，或者静态方法 `Log::<LEVEL>` ，又或是宏定义 `ELG_<LEVEL>`。
* 格式化输出简单，因为格式化输出使用的 [fmt](https://github.com/fmtlib/fmt) 库。
* 自定义格式化方式简单，支持自定义 `formatter`，而且已经预置四种 `formatter`，包括 defaultFormatter、colorfulFormatter、jsonFormatter、customFormatter。

* 配置简单，支持通过 `json` 文件一键读取配置项。
* 引入简单，支持 `cmake` 命令一键引入项目并使用。

性能 `efficiency` 体现在：

* 同步输出一条日志的延迟只需 `180ns` ，异步只需 `120ns`，是 spdlog 至少4倍的性能。

对于benchmark，可以参考[tests/bench_start.cc](https://github.com/ACking-you/elog4cpp/blob/master/tests/bench_start.cc)

## 快速开始

### 要求

* C++11及以上，是跨全平台的

### 安装与引入

推荐用以下两种方式进行引入：

* 方法一：通过cmake中的 `FetchContent` 模块引入：

  1. 在项目的cmake中添加下列代码进行引入，国内如果因为网络问题无法使用可以换这个gitee的镜像源：https://gitee.com/acking-you/elog4cpp.git
      ```cmake
      include(FetchContent)

      FetchContent_Declare(
              elog4cpp
              GIT_REPOSITORY https://github.com/ACking-you/elog4cpp.git
              GIT_TAG origin/master
              GIT_SHALLOW TRUE)
      FetchContent_MakeAvailable(elog4cpp)
      ```
  2. 在需要使用该库的目标中链接 `elog` 即可。
      ```cmake
      target_link_libraries(target  elog)
      ```
* 方法二：手动下载源代码，然后通过cmake命令引入：

  1. 通过git命令下载项目源码

      ```shell
      git clone https://github.com/ACking-you/elog4cpp.git
      ````

  2. 将该项目添加到子项目中：

     ```cmake
     add_subdirectory(elog4cpp)
     ```

  3. 在需要使用该库的目标中链接 `elog` 即可。

     ```cmake
     target_link_libraries(target  elog)
     ```

### 开始使用

* 在不经过任何配置的情况下，我们可以直接调用静态方法输出到终端，代码如下：

    ```cpp
    #include <elog/logger.h>
    using namespace elog;
    
    int main()
    {
        Log::trace("hello elog4cpp");
        Log::debug("hello elog4cpp");
        Log::info("hello elog4cpp");
        Log::warn("hello elog4cpp");
        Log::error("hello elog4cpp");
        Log::fatal("hello elog4cpp");
    }
    ```

   输出结果如下：

   ![output](https://img-blog.csdnimg.cn/15ddda1c5a184e61b54afa83b33dcaa2.png)

   通过上述例子，我们需要明白以下三点：

   1. 本日志库的输出等级一共有 `trace` 、`debug` 、`info` 、`warn` 、`error` 、`fatal` 。
   2. 默认的输出最低输出等级为 `debug` ，也就是 `trace` 等级并不输出。
   3. 在进行 `fatal` 等级的输出时会抛出异常。

   > 实际上在 `error` 或 `fatal` 等级输出时，如果 `errno` 存在错误，那么会输出对应的错误，这在进行系统编程的时候很有用。
---

* 前面的例子中，我们无法输出 `trace` 等级的日志，现在我们尝试着改变它的最低输出等级，然后将输出的内容增加更多的信息（比如文件名、行号、函数名等），并且将输出的内容以颜色高亮的形式输出。

   代码如下：

    ```cpp
    #include <elog/logger.h>
    using namespace elog;
    
    int main()
    {
        GlobalConfig::Get()
            .setLevel(Levels::kTrace)
            .setFormatter(formatter::colorfulFormatter);
        Log::trace(loc::current(),"hello elog4cpp");
        Log::debug(loc::current(),"hello elog4cpp");
        Log::info(loc::current(),"hello elog4cpp");
        Log::warn(loc::current(),"hello elog4cpp");
        Log::error(loc::current(),"hello elog4cpp");
    }
    ```

   输出效果如下：

   ![tupian](https://img-blog.csdnimg.cn/ba02536932f54116a73c74006b6587bf.png)
   从上面的示例代码中，我们发现，如果需要获得文件名等信息，需要在第一个参数中传入 `loc::current()` ，显然大多数时候我们会觉得这样使用起来会很麻烦，所以我们可以通过宏去解决这个问题，你可以像下面这样，在引入 `<elog/logger.h>` 之前定义 `ENABLE_ELG_LOG` 宏来使用更简短的宏定义 `ELG_<LEVEL>`。

    ```cpp
    #define ENABLE_ELG_LOG
    #include <elog/logger.h>
    using namespace elog;
    
    int main()
    {
        GlobalConfig::Get()
            .setLevel(Levels::kTrace)
            .setFormatter(formatter::colorfulFormatter);
        ELG_TRACE("hello elog4cpp");
        ELG_DEBUG("hello elog4cpp");
        ELG_INFO("hello elog4cpp");
        ELG_WARN("hello elog4cpp");
        ELG_ERROR("hello elog4cpp");
    }
    ```

   这个宏定义生成的代码与之前的示例中的代码等效。

---



* 之前的例子都只是输出到控制台，我们现在把内容输出到文件中去。

   代码如下：

    ```cpp
    #define ENABLE_ELG_LOG
    #include <elog/logger.h>
    using namespace elog;
    
    int main()
    {
        GlobalConfig::Get()
            .setFilepath("../log/")
            .setLevel(Levels::kTrace)
            .setFormatter(formatter::colorfulFormatter);
        ELG_TRACE("hello elog4cpp");
        ELG_DEBUG("hello elog4cpp");
        ELG_INFO("hello elog4cpp");
        ELG_WARN("hello elog4cpp");
        ELG_ERROR("hello elog4cpp");
    }
    ```

   上述代码的输出的结果既会输出到文件中也会输出到控制台中，`setFilepath("../log/")` 指定了输出文件的文件夹为上一层级的 `log` 文件夹。注意这里传递的文件路径只能是输出文件的文件夹路径，也就是说文件输出只支持滚动日志。如果参数换为 `"../log"` 则表示输出文件夹路径为上级目录，且输出文件的名字前面都带有 `log` 。输出文件夹的命名格式为：`.<DATE TIME>.<USERNAME>.<PID>.log` 。

   如果需要禁用输出到控制台，则只需要添加下列配置：`GlobalConfig::Get().enableConsole(false)` 。同理如果不需要输出到文件，则需要保持 `log_filepath` 的值为默认值 `nullptr` 即可。

经过以上三次实践，大家应该对本库的基本使用已经有所了解，接下来如果需要详细了解对应的使用方式，则可以以继续深入了解如下内容：

1. [如何配置](#如何配置)
2. [详细接口描述](#详细接口描述)

## 如何配置

所有的配置都基于 `Config` 类或者 `GlobalConfig` 类。请注意这两个类的关系，`Config` 类作为 `GlobalConfig` 的基类，`Config` 中含有一些输出的通用配置，一般用于局部配置，`GlobalConfig` 中含有一些特殊的一次性配置，一般作为全局单例用于全局配置。

### 全局配置

如果没有设置局部配置，默认使用的都是全局配置。如果是使用静态方法或宏进行日志打印，那就只能使用通过全局配置进行配置。

#### 配置方式

所有的全局配置都是通过一个全局的单例 `GlobalConfig` 来进行配置，可以调用 `GlobalConfig::Get` 来获取该单例，共有以下两种方式对该单例进行配置：

1. 调用 `GlobalConfig` 的方法来配置，具体情况如下。
   特有的方法如下：
   * `GlobalConfig::setRollSize(int size)`：设置单个文件最大超过多大，就创建新的文件进行日志打印，以 mb 为基本单位。
   * `GlobalConfig::setFlushInterval(int flushinterval)`：设置每过多长时间进行一次刷新日志到磁盘，以秒为基本单位。
   * `GlobalConfig::setFilepath(const char* basedir)`：设置滚动日志的输出路径，注意传入的并不是单个文件路径，而是一个文件夹路径，本日志库只支持滚动日志。
   * `GlobalConfig::enableConsole(bool s)`：设置是否输出到控制台。
   继承自 `config` 的方法如下：
   * `Config::setFlag(Flags flag)`：设置 flags ，该 flags 可以用于更精细的控制日志输出的内容，对于flags有以下枚举。
     * `kDate`：是否输出日期。
     * `kTime`：是否输出时间。
     * `kLongname`：是否输出长文件名。
     * `kShortname`：是否输出短文件名。
     * `kLine`：是否输出行号。
     * `kFuncName`：是否输出函数名。
     * `kThreadId`：是否输出线程id。
     * `kStdFlags`：代表 `kDate | kTime | kShortname | kLine | kFuncName` ，里面的或运算代表开启对应的功能。
   * `Config::setLevel(Levels level)`：设置最低的日志输出等级。
   * `Config::setName(const char* name)`：设置日志器的名字，会在输出的时候添加该内容。
   * `Config::setBefore(callback_t const& cb)`：设置发生在格式化之前的回调函数。
   * `Config::setAfter(callback_t const& cb)`：设置发生在格式化之后的回调函数。
   * `Config::setFormatter(formatter_t const& formatter)`：设置格式化器，默认已经写好了如下格式化器：
     * `defaultFormatter` ：默认的格式化器。
     * `colorfulFormatter`：在默认格式化器的基础上，在控制台台的输出中带上颜色。
     * `jsonFormatter`：以json格式进行输出。
     * `customFromString(str) -> formatter_t`：这是一个可以你传入的字符串获取自定义的格式化器，具体的是使用方式请查看后续的描述。
2. 通过传入json配置文件来配置，具体情况如下。
   你除了调用对应的方法来进行配置以外，还可以通过外部的json文件进行配置，关键方法在于 `loadFromJSON` 和 `loadToJSON` ，分别用于从 json 文件中读取信息设置 `GlobalConfig` 的变量值和根据 `GlobalConfig` 变量值反过来生成对应的 json 文件。
   具体的 `json` 配置文件如下，所有的使用方式均在 `comments` 中有说明：

   ```json
   {
     "comments": [
       "下面的数值都是默认生成的注释，用于说明参数填写的注意事项",
       "name:可选参数，默认不填则日志输出无name",
       "roll_size:滚动日志的阈值，以mb为单位",
       "flush_interval:日志后台刷盘的时间，以秒为单位",
       "out_console:是否开启输出控制台，是bool值",
       "out_file:是否开启输出日志文件，不开启请使用null值，开启请用一个文件夹目录",
       "flag:用于开启日志对应输出的数据内容，有date,time,line,file,short_file,tid,func七种，可以通过+号来同时开启，当然也可直接使用default，它表示除tid以外的所有选项",
       "level:用于规定全局的最低输出等级，有trace,debug,info,warn,error,fatal,默认使用debug",
       "formatter:用于规定全局的日志格式化方式，有default,colorful,custom这三种，默认采取default，如果使用custom，则需要添加fmt_string",
       "fmt_string:仅当formatter选择custom后用于设定自定义的formatter，对应的数据表示如下：%T:time,%t:tid,%F:filepath,%f:func,%e:error info,%L:long levelText,%l:short levelText,%v:message ,%c color start %C color end"
     ],
     "elog": {
       "flag": "default",
       "flush_interval": 3,
       "formatter": "default",
       "level": "debug",
       "out_console": true,
       "out_file": "null",
       "roll_size": 20
     }
   }
   ```

#### 使用示例

两个简单完整使用示例如下：

1. 通过 `GlobalConfig` 的方法进行配置。

   ```cpp
   #define ENABLE_ELG_LOG
   #include <elog/logger.h>
   using namespace elog;
   
   int main()
   {
   	GlobalConfig::Get()
   		.setRollSize(4)
   		.setFlushInterval(3)
   		.setFilepath("../log/")
   		.enableConsole(true)
   		.setFlag(kStdFlags + kThreadId)
   		.setLevel(kTrace)
   		.setName("elog")
   		.setBefore([](output_buf_t& buf) {
   			buf.append("before");
   		})
   		.setAfter([](output_buf_t& buf) {
   			buf.append("after");
   		})
   		.setFormatter(formatter::customFromString("%c[%L][%T][tid:%t][name:%n][file:%F][func:%f]:%v%C"));
   	ELG_TRACE("hello elog4cpp");
   	ELG_DEBUG("hello elog4cpp");
   	ELG_INFO("hello elog4cpp");
   	ELG_WARN("hello elog4cpp");
   	ELG_ERROR("hello elog4cpp");
   }
   ```

   打印结果如下：
   ![img2](https://img-blog.csdnimg.cn/39e4711a3dd0428a903a4120710d31e2.png)

2. 同理可以直接使用等效的 `json` 配置文件直接加载对应的配置项。
   配置项如下：

   ```cpp
   {
     "elog": {
       "flag": "default+tid",
       "flush_interval": 3,
       "name": "elog",
       "formatter": "custom",
       "fmt_string": "%c[%L][%T][tid:%t][name:%n][file:%F][func:%f]:%v%C",
       "level": "trace",
       "out_console": true,
       "out_file": "../log/",
       "roll_size": 4
     }
   }
   ```

   代码如下：

   ```cpp
   #define ENABLE_ELG_LOG
   #include <elog/logger.h>
   using namespace elog;
   
   int main()
   {
   	GlobalConfig::Get()
   		.loadFromJSON("../config.json")
   		.setBefore([](output_buf_t& buf) {
   			buf.append("before");
   		})
   		.setAfter([](output_buf_t& buf) {
   			buf.append("after");
   		});
   	ELG_TRACE("hello elog4cpp");
   	ELG_DEBUG("hello elog4cpp");
   	ELG_INFO("hello elog4cpp");
   	ELG_WARN("hello elog4cpp");
   	ELG_ERROR("hello elog4cpp");
   }
   ```



### 局部配置

局部配置指的是可以通过单独创建一个类，来使用一个单独的 `Config` 配置。某些配置只提供了全局，具体有`roolSize` 、`flushInterval`、`outConsole`、`outFile`，因为后端负责输出的线程只能是一个单例，所以这些配置也只能配置一次。

关于如何使用局部配置的步骤如下：

1. 通过 `Log::RegisterConfig` 注入 `Config` 。
2. 创建 `Log` 对象并传入当前日志输出的等级以及 `Config` 的名字。
3. 使用 `Log` 对象，调用它对应的 `println` 和 `printf` 方法进行打印。

示例代码如下：

```cpp
#include <elog/logger.h>
#include <memory>
#include <vector>
using namespace elog;

void config_global()
{
	GlobalConfig::Get()
		.loadFromJSON("../config.json")
		.setBefore([](output_buf_t& buf) {
			buf.append("before");
		})
		.setAfter([](output_buf_t& buf) {
			buf.append("after");
		});
}

void register_local_config()
{
	auto config = make_unique<Config>();
	config->log_formatter = formatter::colorfulFormatter;
	config->log_name = "local_config";
	config->log_level = kTrace;
	config->log_flag = kStdFlags + kThreadId;
	config->log_before = [](output_buf_t& buf) {
		buf.append("before");
	};
	config->log_after = [](output_buf_t& buf) {
		buf.append("after");
	};
	// Register Config
	Log::RegisterConfig("local_config",std::move(config));
}

int main()
{
	config_global();
	register_local_config();
	//创建Log对象，设置level，并根据唯一的名字获取已经注册Config
	auto trace = Log(kTrace, "local_config");
	trace.printf("hello {}", "world");
	trace.println("hello ", std::vector<int>{1, 2, 32});
	//改变日志输出等级
	trace.set_level(kDebug);
	trace.printf("hello {}", "world");
	trace.println("hello ", std::vector<int>{1, 2, 32});
	//移动构造到新对象
	auto info = std::move(trace);
	info.set_level(kInfo);
	info.printf("hello {}", "world");
	info.println("hello ", std::vector<int>{1, 2, 32});
}
```
> 注意：注册 Config 的操作不是线程安全的，请确保在进行日志输出之前完成Config的注册。

## 详细接口描述

### Formatter

#### 内置formatter

如果你看过前面的内容，那么对 `formatter` 的作用应该有了一定的了解，他是一个用于控制格式化输出的接口实现，本日志库内部已经实现的 `formatter` 有如下几种：

* `defaultFormatter` ：这是默认的formatter，格式固定。
* `jsonFormatter`：以json格式输出，格式固定。
* `colorfulFormatter`：输出的格式与formatter相同，但输出到控制台的时候有颜色高亮。
* `customFromString`：可以根据用户传入的字符串自定义输出格式。
  具体描述：
  * %T：表示整个日期时间，还包括时区。
  * %t：表示线程id。
  * %F：表示该条日志输出来自哪个文件。
  * %f：表示该条日志输出来自哪个函数。
  * %e：表示如果 `errno` 存在错误则表示该错误信息，否则表示空。
  * %n：表示当前日志器的名字，如果不存在，则表示为空。
  * %L：表示长的代表日志等级的字符串，比如 `TRACE` 。
  * %l：表示短的代表日志等级的字符串，比如 `TRC` 。
  * %v：表示日志输出的内容。
  * %c和%C：表示颜色的开始与结束，只在支持 `\033` 的终端中有效。

#### 自定义formatter

既然想要 `formatter` ，那么就必须清楚 `formatter` 在本日志库中到底被设计成了什么。实际上 `formatter` 在本日志库中只是一个简单的回调函数，函数签名如下：

```cpp
using formatter_t = std::function<void(Config* config, context const& ctx, buffer_t&buf,Appenders apender_type)>;
```

各个参数的含义如下：

* `config`：当前日志输出使用的配置。
* `ctx`：当前日志输出的相关内容，包括需要输出的日志内容、日志等级和行号等等信息。
* `buf`：当前日志最终格式化后需要输出到的 `buffer` 。
* `appder_type`：这是一个枚举代表当前日志格式化输出的目的地，具体有 文件 或 控制台 两种。

根据上述解释，如果想要实现一个自己的 `formatter` ，就可以通过 `config` 的配置信息和 `ctx` 的输出信息以及 `appender_type` 的目的地信息来定制化的格式化输出内容到 `buf` 中。

有了上述理解，我们就能够通过观察原本已经实现的 `formatter` 来仿照的实现自己的 `formatter` 了，例如 `defaultFormatter` 的源码链接如下：[src/formatter.cc](https://github.com/ACking-you/elog4cpp/blob/master/src/formatter.cc)

### Micros

#### ENABLE_ELG_LOG

为了使得文件位置信息的输出不需要手动的添加 `loc::current()` 这一参数，本日志库提供了 `ELG_<LEVEL>` 来简化这一过程，所以如果需要文件位置信息可以将 `Log::<Level>` 替换为下面的宏：

* ELG_TRACE
* ELG_DEBUG
* ELG_INFO
* ELG_WARN
* ELG_ERROR
* ELG_FATAL

> 注意：使用上述宏之前，需要在 `#include<elog/logger.h>` 之前定义 `ENABLE_ELG_LOG` 宏。

#### ENABLE_ELG_CHECK

通过定义 `ENABLE_ELG_CHECK` 宏，我们可以使用到如下的宏定义来更方便的检查值之间的关系：
* 断言宏，不满足条件，则抛出异常。
  * `ELG_CHECK_EQ(a,b)` 等价于 `ELG_ASSERT_IF(a == b)`.
  * `ELG_CHECK_NQ(a,b)` 等价于 `ELG_ASSERT_IF(a != b)`.
  * `ELG_CHECK_GE(a,b)` 等价于 `ELG_ASSERT_IF(a > b)`.
  * `ELG_CHECK_GT(a,b)` 等价于 `ELG_ASSERT_IF(a >= b)`.
  * `ELG_CHECK_LE(a,b)` 等价于 `ELG_ASSERT_IF(a < b)`.
  * `ELG_CHECK_LT(a,b)` 等价于 `ELG_ASSERT_IF(a <= b)`.
  * `ELG_CHECK_NOTNULL(a)` 等价于 `ELG_ASSERT_IF(a != nullptr)`.
* 判断断言，自定义打印。
   在传入判断条件后，会返回一个对象供你进行打印提示信息，可以选择不同的等级进行打印，如下面的例子是打印 `trace` 等级。
   ```cpp
   ELG_CHECK(1 == 2).trace("1 != 2");
   ```

### 其他杂项

还有如下提供方便的函数：
* `elog::Ptr`：用于将任意指针强制转化为 `void*` ，这是为了方便直接打印指针的值。
* `elog::WaitForDone`：等待后台线程将日志信息刷入磁盘，这在某些时候很有用。