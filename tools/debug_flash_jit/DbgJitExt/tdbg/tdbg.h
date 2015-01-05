// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 TDBG_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// TDBG_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef TDBG_EXPORTS
#define TDBG_API __declspec(dllexport)
#else
#define TDBG_API __declspec(dllimport)
#endif

// 此类是从 tdbg.dll 导出的
class TDBG_API Ctdbg {
public:
	Ctdbg(void);
	// TODO: 在此添加您的方法。
};

extern TDBG_API int ntdbg;

TDBG_API int fntdbg(void);

#define INIT_API()                             \
	HRESULT Status;                            \
	if ((Status = ExtQuery(Client)) != S_OK) return Status; 


#define EXT_RELEASE(Unk) \
	((Unk) != NULL ? ((Unk)->Release(), (Unk) = NULL) : NULL)

#define EXIT_API ExtRelease

void ExtRelease(void);