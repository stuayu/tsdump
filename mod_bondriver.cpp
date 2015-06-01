#include <Windows.h>
#include <stdio.h>

#include "modules_def.h"
#include "IBonDriver2.h"

typedef struct {
	HMODULE hdll;
	pCreateBonDriver_t *pCreateBonDriver;
	IBonDriver *pBon;
	IBonDriver2 *pBon2;
} bondriver_stat_t;

WCHAR errmsg[1024];

static const WCHAR *bon_dll_name = NULL;
static int sp_num = -1;
static int ch_num = -1;

static const WCHAR *reg_hook_msg;

LPWSTR lasterr_msg()
{
	LPWSTR msg;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)(&msg),
		0,
		NULL
	);
	return msg;
}

const WCHAR* hook_postconfig()
{
	if (bon_dll_name == NULL) {
		return NULL;
	}

	if (reg_hook_msg != NULL) {
		_snwprintf_s(errmsg, 1024 - 1, L"generator�t�b�N�̓o�^�Ɏ��s���܂���: %s", reg_hook_msg);
		return errmsg;
	}

	if (ch_num < 0) {
		return L"�`�����l�����w�肳��Ă��Ȃ����A�܂��͕s���ł�";
	}
	if (sp_num < 0) {
		return L"�`���[�i�[��Ԃ��w�肳��Ă��Ȃ����A�܂��͕s���ł�";
	}

	return NULL;
}

static void hook_stream_generator(void*, unsigned char **buf, int *size)
{
}

static void* hook_stream_generator_open(ch_info_t *chinfo)
{
	LPWSTR msg;
	bondriver_stat_t *pstat, stat;
	ch_info_t ci;

	stat.hdll = LoadLibrary(bon_dll_name);
	if (stat.hdll == NULL) {
		fprintf(stderr, "BonDriver�����[�h�ł��܂���ł���\n");
		print_err(L"LoadLibrary", GetLastError());
		return NULL;
	}

	stat.pCreateBonDriver = (pCreateBonDriver_t*)GetProcAddress(stat.hdll, "CreateBonDriver");
	if (stat.pCreateBonDriver == NULL) {
		fprintf(stderr, "CreateBonDriver()�̃|�C���^���擾�ł��܂���ł���\n");
		print_err(L"GetProcAddress", GetLastError());
		FreeLibrary(stat.hdll);
		return NULL;
	}

	stat.pBon = stat.pCreateBonDriver();
	if (stat.pBon == NULL) {
		fprintf(stderr, "CreateBonDriver() returns NULL\n" );
		FreeLibrary(stat.hdll);
		return NULL;
	}

	stat.pBon2 = dynamic_cast<IBonDriver2 *>(stat.pBon);

	if (! stat.pBon2->OpenTuner()) {
		fprintf(stderr, "OpenTuner() returns FALSE\n" );
		FreeLibrary(stat.hdll);
		return NULL;
	}

	ci.ch_str = stat.pBon2->EnumChannelName(sp_num, ch_num);
	ci.sp_str = stat.pBon2->EnumTuningSpace(sp_num);
	ci.tuner_name = stat.pBon2->GetTunerName();
	ci.ch_num = ch_num;
	ci.sp_num = sp_num;

	wprintf(L"BonTuner: %s\n", ci.tuner_name);
	wprintf(L"Space: %s\n", ci.sp_str);
	wprintf(L"Channel: %s\n", ci.ch_str);
	if (!stat.pBon2->SetChannel(sp_num, ch_num)) {
		fprintf(stderr, "SetChannel() returns FALSE\n");
		stat.pBon2->CloseTuner();
		FreeLibrary(stat.hdll);
		return NULL;
	}

	*chinfo = ci;
	pstat = (bondriver_stat_t*)malloc(sizeof(bondriver_stat_t));
	*pstat = stat;
	return pstat;
}

static double hook_stream_generator_siglevel(void*)
{
	return 0.0;
}

static void hook_stream_generator_close(void*)
{
}

static hooks_stream_generator_t hooks_stream_generator = {
	hook_stream_generator,
	hook_stream_generator_open,
	hook_stream_generator_siglevel,
	hook_stream_generator_close
};

static void hook_close_module()
{
}

static void register_hooks()
{
	if (bon_dll_name != NULL) {
		reg_hook_msg = register_hooks_stream_generator(&hooks_stream_generator);
	}
	register_hook_close_module(hook_close_module);
	register_hook_postconfig(hook_postconfig);
}

static const WCHAR *set_bon(const WCHAR* param)
{
	bon_dll_name = _wcsdup(param);
	return NULL;
}

static const WCHAR* set_sp(const WCHAR *param)
{
	sp_num = _wtoi(param);
	if (sp_num < 0) {
		return L"�X�y�[�X�ԍ����s���ł�";
	}
	return NULL;
}

static const WCHAR* set_ch(const WCHAR *param)
{
	ch_num = _wtoi(param);
	if (ch_num < 0) {
		return L"�`�����l���ԍ����s���ł�";
	}
	return NULL;
}

static cmd_def_t cmds[] = {
	{ L"-bon", L"BonDriver��DLL*", 1, set_bon },
	{ L"-sp", L"�`���[�i�[��Ԕԍ�*", 1, set_sp },
	{ L"-ch", L"�`�����l���ԍ�*", 1, set_ch },
	NULL,
};

MODULE_DEF module_def_t mod_bondriver = {
	TSDUMP_MODULE_V2,
	L"mod_bondriver",
	register_hooks,
	cmds
};