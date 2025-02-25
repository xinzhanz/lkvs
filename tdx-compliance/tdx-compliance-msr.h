/* SPDX-License-Identifier: GPL-2.0-only */
#include "tdx-compliance.h"

#define NO_EXCP 0
#define NO_PRE_COND NULL

#define DEF_MSR(_name, _msr_num, _rw, _excp, _precond, _size, _vsn)	\
{							\
	.name = _name "_" #_rw,			\
	.version = _vsn,				\
	.msr.msr_num = _msr_num,			\
	.run_msr_rw = _rw##_msr##_native,		\
	.excp.expect = _excp,				\
	.pre_condition = _precond,			\
	.size = _size,					\
}

#define DEF_READ_MSR(_msr_num, _excp, _precond, _vsn)			\
	DEF_MSR(#_msr_num, _msr_num, read, _excp, _precond, 1, _vsn)

#define DEF_WRITE_MSR(_msr_num, _excp, _precond, _vsn)			\
	DEF_MSR(#_msr_num, _msr_num, write, _excp, _precond, 1, _vsn)

#define DEF_READ_MSR_SIZE(_msr_num, _excp, _precond, _size, _vsn)	\
	DEF_MSR(#_msr_num, _msr_num, read, _excp, _precond, _size, _vsn)

#define DEF_WRITE_MSR_SIZE(_msr_num, _excp, _precond, _size, _vsn)	\
	DEF_MSR(#_msr_num, _msr_num, write, _excp, _precond, _size, _vsn)

#define MSR_IA32_MKTME_PARTITIONING 0x87
#define MSR_WBINVDP 0x98
#define MSR_WBNOINVDP 0x99
#define MSR_INTR_PENDING 0x9a
#define IA32_MISC_PACKAGE_CTLS 0xbc
#define IA32_PLATFORM_DCA_CAP 0x01f8
#define IA32_CPU_DCA_CAP 0x01f9
#define IA32_DCA_0_CAP 0x01fa
#define MSR_SLAM_ENABLE 0x0276
#define IA32_PERF_GLOBAL_STATUS_SET 0x0391
#define IA32_PERF_GLOBAL_INUSE 0x0392
#define IA32_SGX_SVN_STATUS 0x0500
#define IA32_SE_SVN_EXPAN 0x0501
#define IA32_PKRS 0x06e1
#define RESERVED_XAPIC_0X0800 0x0800
#define IA32_X2APIC_APICID 0x0802
#define IA32_X2APIC_VERSION 0x0803
#define RESERVED_XAPIC_0X0804 0x0804
#define IA32_X2APIC_TPR 0x0808
#define RESERVED_XAPIC_0X0809 0x0809
#define IA32_X2APIC_PPR 0x080a
#define IA32_X2APIC_EOI 0x080b
#define RESERVED_XAPIC_0X080C 0x080c
#define RESERVED_XAPIC_0X080E 0x080e
#define IA32_X2APIC_ISRX 0x0810
#define IA32_X2APIC_TMRX 0x0818
#define IA32_X2APIC_IRRX 0x0820
#define RESERVED_XAPIC_0X0829 0x0829
#define RESERVED_XAPIC_0X0831 0x0831
#define IA32_X2APIC_SELF_IPI 0x083f
#define RESERVED_XAPIC_0X0840 0x0840
#define RESERVED_XAPIC_0X0880 0x0880
#define RESERVED_XAPIC_0X08C0 0x08c0
#define IA32_TME_CAPABILITIES 0x0981
#define IA32_TME_ACTIVATE 0x0982
#define IA32_TME_EXCLUDE_MASK 0x0983
#define IA32_TME_EXCLUDE_BASE 0x0984
#define IA32_UINT_RR 0x0985
#define IA32_UINT_HANDLER 0x0986
#define IA32_UINT_STACKADJUST 0x0987
#define IA32_UINT_MISC 0x0988
#define IA32_UINT_PD 0x0989
#define IA32_UINT_TT 0x098a
#define IA32_DEBUG_INTERFACE 0x0c80
#define IA32_UARCH_MISC_CTL 0x1b01

/* pre-conditions */
static int get_perfmon(void) {
	/* PERFMON: CPUID(0xa).* */
	struct test_cpuid cpt = DEF_CPUID_TEST(0xa, 0);

	run_cpuid(&cpt);

	if (cpt.regs.eax.val == 0 && cpt.regs.ebx.val == 0 &&
	    cpt.regs.ecx.val == 0 && cpt.regs.edx.val == 0)
		return 0;

	return 1;
}

static void pre_perfmon(struct test_msr *c)
{
	if (!get_perfmon())
		c->excp.expect = X86_TRAP_GP;
}

static void pre_pks(struct test_msr *c)
{
	/* PKS: CPUID(0x7,0x0).ecx[31] */
	struct test_cpuid cpt = DEF_CPUID_TEST(0x7, 0);

	run_cpuid(&cpt);

	if ((cpt.regs.ecx.val & _BITUL(31)) == 0)
		c->excp.expect = X86_TRAP_GP;
}

static void pre_0x1_ecx24(struct test_msr *c)
{
	/* CPUID(0x1,0x0).ecx[24] */
	struct test_cpuid cpt = DEF_CPUID_TEST(0x1, 0);

	run_cpuid(&cpt);

	if ((cpt.regs.ecx.val & _BITUL(24)) == 0)
		c->excp.expect = X86_TRAP_GP;
}

static void pre_0x7_ecx5(struct test_msr *c)
{
	/* CPUID(0x7,0x0).ecx[5] */
	struct test_cpuid cpt = DEF_CPUID_TEST(0x7, 0);

	run_cpuid(&cpt);

	if ((cpt.regs.ecx.val & _BITUL(5)) == 0)
		c->excp.expect = X86_TRAP_GP;
}

static void pre_0x7_ecx13(struct test_msr *c)
{
	/* CPUID(0x7,0x0).ecx[13] */
	struct test_cpuid cpt = DEF_CPUID_TEST(0x7, 0);

	run_cpuid(&cpt);

	if ((cpt.regs.ecx.val & _BITUL(13)) == 0)
		c->excp.expect = X86_TRAP_GP;
}

static void pre_0x7_edx18(struct test_msr *c)
{
	/* CPUID(0x7,0x0).edx[18] */
	struct test_cpuid cpt = DEF_CPUID_TEST(0x7, 0);

	run_cpuid(&cpt);

	if ((cpt.regs.edx.val & _BITUL(18)) == 0)
		c->excp.expect = X86_TRAP_GP;
}

static void pre_0xd_0x1_eax4(struct test_msr *c)
{
	/* CPUID(0xd,0x1).eax[4] */
	struct test_cpuid cpt = DEF_CPUID_TEST(0xd, 1);

	run_cpuid(&cpt);

	if ((cpt.regs.eax.val & _BITUL(4)) == 0)
		c->excp.expect = X86_TRAP_GP;
}

static void pre_0x1_ecx18(struct test_msr *c)
{
	/* CPUID(0x1).ecx[18] */
	struct test_cpuid cpt = DEF_CPUID_TEST(0x1, 0);

	run_cpuid(&cpt);

	if ((cpt.regs.ecx.val & _BITUL(18)) == 0)
		c->excp.expect = X86_TRAP_GP;
}

static void pre_xfam_8(struct test_msr *c)
{
	/* XFAM[8] RTIT CPUID(0x7,0x0).ebx[25] */
	struct test_cpuid cpt = DEF_CPUID_TEST(0x7, 0);

	run_cpuid(&cpt);

	if ((cpt.regs.ebx.val & _BITUL(25)) == 0)
		c->excp.expect = X86_TRAP_GP;
}

static void pre_xfam_12_11(struct test_msr *c)
{
	/* XFAM[12:11] CET CPUID(0xd,0x1).ecx[12:11] */
	struct test_cpuid cpt = DEF_CPUID_TEST(0xd, 1);

	run_cpuid(&cpt);

	if ((cpt.regs.ecx.val & _BITUL(11)) == 0 &&
	    (cpt.regs.ecx.val & _BITUL(12)) == 0)
		c->excp.expect = X86_TRAP_GP;
}

static void pre_xfam_14(struct test_msr *c)
{
	/* XFAM[14] ULI CPUID(0x7,0x0).edx[5] */
	struct test_cpuid cpt = DEF_CPUID_TEST(0x7, 0);

	run_cpuid(&cpt);

	if ((cpt.regs.edx.val & _BITUL(5)) == 0)
		c->excp.expect = X86_TRAP_GP;
}

static void pre_xfam_15(struct test_msr *c)
{
	/* XFAM[15] LBR CPUID(0x7,0x0).edx[19] */
	struct test_cpuid cpt = DEF_CPUID_TEST(0x7, 0);

	run_cpuid(&cpt);

	if ((cpt.regs.edx.val & _BITUL(19)) == 0)
		c->excp.expect = X86_TRAP_GP;
}

static void pre_tsx(struct test_msr *c)
{
	/* TSX enabled: IA32_ARCH_CAPABILITIES[7] */
	struct test_msr t = DEF_READ_MSR(MSR_IA32_ARCH_CAPABILITIES, NO_EXCP, NO_PRE_COND, VER1_0);

	if (!read_msr_native(&t)) {
		if ((t.msr.val.q & _BITUL(7)) == 0)
			c->excp.expect = X86_TRAP_GP;
	}
}

static void pre_fixedctr(struct test_msr *c)
{
	struct test_cpuid cpt = DEF_CPUID_TEST(0xa, 0x0);

	run_cpuid(&cpt);

	if (!get_perfmon() || (cpt.regs.edx.val & 0x1f) == 0)
		c->excp.expect = X86_TRAP_GP;
}

struct test_msr msr_cases[] = {
	DEF_READ_MSR(MSR_IA32_PLATFORM_ID, X86_TRAP_VE, NO_PRE_COND, VER1_5| VER2_0),
	DEF_WRITE_MSR(MSR_IA32_PLATFORM_ID, X86_TRAP_VE, NO_PRE_COND, VER1_5| VER2_0),
	/* according to MSR Index & Name, Ihis is a Guest behavior. */
	DEF_READ_MSR(MSR_IA32_APICBASE, NO_EXCP, NO_PRE_COND, VER1_5| VER2_0),
	DEF_WRITE_MSR(MSR_IA32_APICBASE, X86_TRAP_VE, NO_PRE_COND, VER1_5| VER2_0),
	DEF_READ_MSR(MSR_TEST_CTRL, X86_TRAP_VE, NO_PRE_COND, VER1_5| VER2_0),
	DEF_WRITE_MSR(MSR_TEST_CTRL, X86_TRAP_VE, NO_PRE_COND, VER1_5| VER2_0),
	DEF_READ_MSR(MSR_IA32_TSC_ADJUST, X86_TRAP_VE, NO_PRE_COND, VER1_5| VER2_0),
	DEF_WRITE_MSR(MSR_IA32_TSC_ADJUST, X86_TRAP_VE, NO_PRE_COND, VER1_5| VER2_0),
	DEF_READ_MSR(MSR_IA32_TSC, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_TSC, X86_TRAP_VE, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_SPEC_CTRL, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_SPEC_CTRL, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_PRED_CMD, NO_EXCP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_PRED_CMD, X86_TRAP_GP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_PRED_CMD, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_MKTME_PARTITIONING, X86_TRAP_VE, pre_0x7_edx18, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_MKTME_PARTITIONING, X86_TRAP_VE, pre_0x7_edx18, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR_SIZE(MSR_IA32_SGXLEPUBKEYHASH0, X86_TRAP_GP, NO_PRE_COND, 0x4, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(MSR_IA32_SGXLEPUBKEYHASH0, X86_TRAP_GP, NO_PRE_COND, 0x4, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_WBINVDP, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_WBINVDP, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_WBNOINVDP, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_WBNOINVDP, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_INTR_PENDING, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_INTR_PENDING, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_SMM_MONITOR_CTL, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_SMM_MONITOR_CTL, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_SMBASE, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_SMBASE, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_MISC_PACKAGE_CTLS, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_MISC_PACKAGE_CTLS, X86_TRAP_VE, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_READ_MSR_SIZE(MSR_IA32_PERFCTR0, NO_EXCP, pre_perfmon, 0x8, VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(MSR_IA32_PERFCTR0, NO_EXCP, pre_perfmon, 0x8, VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_UMWAIT_CONTROL, NO_EXCP, pre_0x7_ecx5, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_UMWAIT_CONTROL, NO_EXCP, pre_0x7_ecx5, VER1_0 | VER1_5 | VER2_0),

	DEF_WRITE_MSR(MSR_IA32_ARCH_CAPABILITIES, NO_EXCP, NO_PRE_COND, VER1_0),
	DEF_WRITE_MSR(MSR_IA32_ARCH_CAPABILITIES, X86_TRAP_GP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_FLUSH_CMD, NO_EXCP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_FLUSH_CMD, X86_TRAP_GP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_FLUSH_CMD, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_TSX_CTRL, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_WRITE_MSR(MSR_IA32_TSX_CTRL, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_TSX_CTRL, NO_EXCP, pre_tsx, VER1_5),
	DEF_WRITE_MSR(MSR_IA32_TSX_CTRL, NO_EXCP, pre_tsx, VER1_5),
	DEF_READ_MSR(MSR_IA32_TSX_CTRL, NO_EXCP, NO_PRE_COND, VER2_0),
	DEF_WRITE_MSR(MSR_IA32_TSX_CTRL, NO_EXCP, NO_PRE_COND, VER2_0),
	DEF_READ_MSR(MSR_IA32_SYSENTER_CS, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_SYSENTER_CS, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_SYSENTER_ESP, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_SYSENTER_ESP, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_SYSENTER_EIP, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_SYSENTER_EIP, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_READ_MSR_SIZE(MSR_P6_EVNTSEL0, NO_EXCP, pre_perfmon, 0x8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(MSR_P6_EVNTSEL0, NO_EXCP, pre_perfmon, 0x8, VER1_0 | VER1_5 | VER2_0),

	DEF_WRITE_MSR(MSR_IA32_MISC_ENABLE, X86_TRAP_VE, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR_SIZE(MSR_OFFCORE_RSP_0, NO_EXCP, pre_perfmon, 0x2, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(MSR_OFFCORE_RSP_0, NO_EXCP, pre_perfmon, 0x2, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_XFD, NO_EXCP, pre_0xd_0x1_eax4, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_XFD, NO_EXCP, pre_0xd_0x1_eax4, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_XFD_ERR, NO_EXCP, pre_0xd_0x1_eax4, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_XFD_ERR, NO_EXCP, pre_0xd_0x1_eax4, VER1_0 | VER1_5 | VER2_0),

	DEF_READ_MSR(IA32_PLATFORM_DCA_CAP, X86_TRAP_VE, pre_0x1_ecx18, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_PLATFORM_DCA_CAP, X86_TRAP_VE, pre_0x1_ecx18, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_CPU_DCA_CAP, X86_TRAP_VE, pre_0x1_ecx18, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_CPU_DCA_CAP, X86_TRAP_VE, pre_0x1_ecx18, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_DCA_0_CAP, X86_TRAP_VE, pre_0x1_ecx18, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_DCA_0_CAP, X86_TRAP_VE, pre_0x1_ecx18, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_SLAM_ENABLE, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_SLAM_ENABLE, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_CR_PAT, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_CR_PAT, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),

//Workaround for MSR_CORE_PERF_FIXED_CTR0
//	DEF_READ_MSR_SIZE(MSR_CORE_PERF_FIXED_CTR0, NO_EXCP, pre_fixedctr, 0x8, VER1_0 | VER1_5),
//	DEF_WRITE_MSR_SIZE(MSR_CORE_PERF_FIXED_CTR0, NO_EXCP, pre_fixedctr, 0x8, VER1_0 | VER1_5),
//	DEF_READ_MSR_SIZE(MSR_CORE_PERF_FIXED_CTR0, NO_EXCP, pre_fixedctr, 0x4, VER2_0),
//	DEF_WRITE_MSR_SIZE(MSR_CORE_PERF_FIXED_CTR0, NO_EXCP, pre_fixedctr, 0x4, VER2_0),
	DEF_READ_MSR_SIZE(MSR_CORE_PERF_FIXED_CTR0, NO_EXCP, pre_fixedctr, 0x1, VER1_0 | VER1_5),
	DEF_WRITE_MSR_SIZE(MSR_CORE_PERF_FIXED_CTR0, NO_EXCP, pre_fixedctr, 0x1, VER1_0 | VER1_5),
	DEF_READ_MSR_SIZE(MSR_CORE_PERF_FIXED_CTR0, NO_EXCP, pre_fixedctr, 0x1, VER2_0),
	DEF_WRITE_MSR_SIZE(MSR_CORE_PERF_FIXED_CTR0, NO_EXCP, pre_fixedctr, 0x1, VER2_0),
	DEF_READ_MSR(MSR_PERF_METRICS, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_PERF_METRICS, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),

	DEF_WRITE_MSR(MSR_IA32_PERF_CAPABILITIES, X86_TRAP_GP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_CORE_PERF_FIXED_CTR_CTRL, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_CORE_PERF_FIXED_CTR_CTRL, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_CORE_PERF_GLOBAL_STATUS, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_CORE_PERF_GLOBAL_STATUS, X86_TRAP_GP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_CORE_PERF_GLOBAL_CTRL, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_CORE_PERF_GLOBAL_CTRL, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_CORE_PERF_GLOBAL_OVF_CTRL, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_CORE_PERF_GLOBAL_OVF_CTRL, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_PERF_GLOBAL_STATUS_SET, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_PERF_GLOBAL_STATUS_SET, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_PERF_GLOBAL_INUSE, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_PERF_GLOBAL_INUSE, X86_TRAP_GP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_PEBS_ENABLE, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_PEBS_ENABLE, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_PEBS_DATA_CFG, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_PEBS_DATA_CFG, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_PEBS_LD_LAT_THRESHOLD, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_PEBS_LD_LAT_THRESHOLD, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_PEBS_FRONTEND, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_PEBS_FRONTEND, NO_EXCP, pre_perfmon, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_BASIC, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_BASIC, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_BASIC, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_PINBASED_CTLS, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_PINBASED_CTLS, X86_TRAP_VE, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_PINBASED_CTLS, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_PROCBASED_CTLS, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_PROCBASED_CTLS, X86_TRAP_VE, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_PROCBASED_CTLS, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_EXIT_CTLS, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_EXIT_CTLS, X86_TRAP_VE, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_EXIT_CTLS, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_ENTRY_CTLS, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_ENTRY_CTLS, X86_TRAP_VE, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_ENTRY_CTLS, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_MISC, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_MISC, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_MISC, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_CR0_FIXED0, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_CR0_FIXED0, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_CR0_FIXED0, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_CR0_FIXED1, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_CR0_FIXED1, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_CR0_FIXED1, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_CR4_FIXED0, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_CR4_FIXED0, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_CR4_FIXED0, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_CR4_FIXED1, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_CR4_FIXED1, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_CR4_FIXED1, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_VMCS_ENUM, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_VMCS_ENUM, X86_TRAP_VE, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_VMCS_ENUM, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_PROCBASED_CTLS2, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_PROCBASED_CTLS2, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_PROCBASED_CTLS2, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_EPT_VPID_CAP, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_EPT_VPID_CAP, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_EPT_VPID_CAP, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_TRUE_PINBASED_CTLS, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_TRUE_PINBASED_CTLS, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_TRUE_PINBASED_CTLS, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_TRUE_PROCBASED_CTLS, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_TRUE_PROCBASED_CTLS, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_TRUE_PROCBASED_CTLS, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_TRUE_EXIT_CTLS, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_TRUE_EXIT_CTLS, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_TRUE_EXIT_CTLS, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_TRUE_ENTRY_CTLS, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_TRUE_ENTRY_CTLS, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_TRUE_ENTRY_CTLS, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_VMFUNC, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_VMFUNC, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_VMFUNC, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_VMX_PROCBASED_CTLS3, X86_TRAP_GP, NO_PRE_COND, VER1_0),
	DEF_READ_MSR(MSR_IA32_VMX_PROCBASED_CTLS3, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_VMX_PROCBASED_CTLS3, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR_SIZE(MSR_IA32_PMC0, NO_EXCP, pre_perfmon, 0x8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(MSR_IA32_PMC0, NO_EXCP, pre_perfmon, 0x8, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_SGX_SVN_STATUS, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_SGX_SVN_STATUS, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),

	DEF_READ_MSR(IA32_SE_SVN_EXPAN, X86_TRAP_VE, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_SE_SVN_EXPAN, X86_TRAP_VE, NO_PRE_COND, VER1_5 | VER2_0),

	DEF_READ_MSR(MSR_IA32_RTIT_OUTPUT_BASE, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_RTIT_OUTPUT_BASE, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_RTIT_OUTPUT_MASK, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_RTIT_OUTPUT_MASK, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_RTIT_CTL, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_RTIT_CTL, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_RTIT_STATUS, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_RTIT_STATUS, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_RTIT_CR3_MATCH, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_RTIT_CR3_MATCH, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_RTIT_ADDR0_A, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_RTIT_ADDR0_A, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_RTIT_ADDR0_B, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_RTIT_ADDR0_B, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_RTIT_ADDR1_A, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_RTIT_ADDR1_A, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_RTIT_ADDR1_B, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_RTIT_ADDR1_B, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_RTIT_ADDR2_A, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_RTIT_ADDR2_A, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_RTIT_ADDR2_B, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_RTIT_ADDR2_B, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_RTIT_ADDR3_A, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_RTIT_ADDR3_A, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_RTIT_ADDR3_B, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_RTIT_ADDR3_B, NO_EXCP, pre_xfam_8, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_DS_AREA, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_DS_AREA, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_U_CET, NO_EXCP, pre_xfam_12_11, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_U_CET, NO_EXCP, pre_xfam_12_11, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_S_CET, NO_EXCP, pre_xfam_12_11, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_S_CET, NO_EXCP, pre_xfam_12_11, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_PL0_SSP, NO_EXCP, pre_xfam_12_11, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_PL0_SSP, NO_EXCP, pre_xfam_12_11, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_PL1_SSP, NO_EXCP, pre_xfam_12_11, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_PL1_SSP, NO_EXCP, pre_xfam_12_11, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_PL2_SSP, NO_EXCP, pre_xfam_12_11, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_PL2_SSP, NO_EXCP, pre_xfam_12_11, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_PL3_SSP, NO_EXCP, pre_xfam_12_11, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_PL3_SSP, NO_EXCP, pre_xfam_12_11, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_INT_SSP_TAB, NO_EXCP, pre_xfam_12_11, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_INT_SSP_TAB, NO_EXCP, pre_xfam_12_11, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_TSC_DEADLINE, X86_TRAP_VE, pre_0x1_ecx24, VER1_5),
	DEF_WRITE_MSR(MSR_IA32_TSC_DEADLINE, X86_TRAP_VE, pre_0x1_ecx24, VER1_5),
	/* according to MSR Index & Name, Ihis is a Guest behavior. */
	DEF_READ_MSR(MSR_IA32_TSC_DEADLINE, NO_EXCP, NO_PRE_COND, VER2_0),
	DEF_WRITE_MSR(MSR_IA32_TSC_DEADLINE, NO_EXCP, NO_PRE_COND, VER2_0),
	DEF_READ_MSR(IA32_PKRS, NO_EXCP, pre_pks, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_PKRS, NO_EXCP, pre_pks, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR_SIZE(RESERVED_XAPIC_0X0800, X86_TRAP_GP, NO_PRE_COND, 0x2, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(RESERVED_XAPIC_0X0800, X86_TRAP_GP, NO_PRE_COND, 0x2, VER1_0 | VER1_5 | VER2_0),
	/* according to MSR Index & Name, Ihis is a Guest behavior. */
	DEF_READ_MSR(IA32_X2APIC_APICID, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_X2APIC_APICID, X86_TRAP_VE, NO_PRE_COND, VER1_5 | VER2_0),
	/* according to MSR Index & Name, Ihis is a Guest behavior. */
	DEF_READ_MSR(IA32_X2APIC_VERSION, NO_EXCP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_X2APIC_VERSION, X86_TRAP_VE, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_READ_MSR_SIZE(RESERVED_XAPIC_0X0804, X86_TRAP_GP, NO_PRE_COND, 0x4, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(RESERVED_XAPIC_0X0804, X86_TRAP_GP, NO_PRE_COND, 0x4, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_X2APIC_TPR, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_X2APIC_TPR, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
//	DEF_READ_MSR(RESERVED_XAPIC_0X0809, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
//	DEF_WRITE_MSR(RESERVED_XAPIC_0X0809, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_X2APIC_PPR, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_X2APIC_PPR, NO_EXCP, NO_PRE_COND, VER1_0),
	DEF_WRITE_MSR(IA32_X2APIC_PPR, X86_TRAP_GP, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_X2APIC_EOI, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_X2APIC_EOI, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
//	DEF_READ_MSR(RESERVED_XAPIC_0X080C, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
//	DEF_WRITE_MSR(RESERVED_XAPIC_0X080C, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
//	DEF_READ_MSR(RESERVED_XAPIC_0X080E, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
//	DEF_WRITE_MSR(RESERVED_XAPIC_0X080E, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR_SIZE(IA32_X2APIC_ISRX, NO_EXCP, NO_PRE_COND, 0x8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(IA32_X2APIC_ISRX, NO_EXCP, NO_PRE_COND, 0x8, VER1_0),
	DEF_WRITE_MSR_SIZE(IA32_X2APIC_ISRX, X86_TRAP_GP, NO_PRE_COND, 0x8, VER1_5 | VER2_0),
	DEF_READ_MSR_SIZE(IA32_X2APIC_TMRX, NO_EXCP, NO_PRE_COND, 0x8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(IA32_X2APIC_TMRX, NO_EXCP, NO_PRE_COND, 0x8, VER1_0),
	DEF_WRITE_MSR_SIZE(IA32_X2APIC_TMRX, X86_TRAP_GP, NO_PRE_COND, 0x8, VER1_5 | VER2_0),
	DEF_READ_MSR_SIZE(IA32_X2APIC_IRRX, NO_EXCP, NO_PRE_COND, 0x8, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(IA32_X2APIC_IRRX, NO_EXCP, NO_PRE_COND, 0x8, VER1_0),
	DEF_WRITE_MSR_SIZE(IA32_X2APIC_IRRX, X86_TRAP_GP, NO_PRE_COND, 0x8, VER1_5 | VER2_0),
	DEF_READ_MSR_SIZE(RESERVED_XAPIC_0X0829, X86_TRAP_GP, NO_PRE_COND, 0x6, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(RESERVED_XAPIC_0X0829, X86_TRAP_GP, NO_PRE_COND, 0x6, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(RESERVED_XAPIC_0X0831, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(RESERVED_XAPIC_0X0831, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_X2APIC_SELF_IPI, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	/* DEF_WRITE_MSR(IA32_X2APIC_SELF_IPI, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	 * this case will crash the guestTD.
	 */
	DEF_READ_MSR_SIZE(RESERVED_XAPIC_0X0840, X86_TRAP_GP, NO_PRE_COND, 0x40, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(RESERVED_XAPIC_0X0840, X86_TRAP_GP, NO_PRE_COND, 0x40, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR_SIZE(RESERVED_XAPIC_0X0880, X86_TRAP_GP, NO_PRE_COND, 0x40, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(RESERVED_XAPIC_0X0880, X86_TRAP_GP, NO_PRE_COND, 0x40, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR_SIZE(RESERVED_XAPIC_0X08C0, X86_TRAP_GP, NO_PRE_COND, 0x40, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(RESERVED_XAPIC_0X08C0, X86_TRAP_GP, NO_PRE_COND, 0x40, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_TME_CAPABILITIES, X86_TRAP_VE, pre_0x7_ecx13, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_TME_CAPABILITIES, X86_TRAP_VE, pre_0x7_ecx13, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_TME_ACTIVATE, X86_TRAP_VE, pre_0x7_ecx13, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_TME_ACTIVATE, X86_TRAP_VE, pre_0x7_ecx13, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_TME_EXCLUDE_MASK, X86_TRAP_VE, pre_0x7_ecx13, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_TME_EXCLUDE_MASK, X86_TRAP_VE, pre_0x7_ecx13, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_TME_EXCLUDE_BASE, X86_TRAP_VE, pre_0x7_ecx13, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_TME_EXCLUDE_BASE, X86_TRAP_VE, pre_0x7_ecx13, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_UINT_RR, NO_EXCP, pre_xfam_14, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_UINT_RR, NO_EXCP, pre_xfam_14, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_UINT_HANDLER, NO_EXCP, pre_xfam_14, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_UINT_HANDLER, NO_EXCP, pre_xfam_14, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_UINT_STACKADJUST, NO_EXCP, pre_xfam_14, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_UINT_STACKADJUST, NO_EXCP, pre_xfam_14, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_UINT_MISC, NO_EXCP, pre_xfam_14, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_UINT_MISC, NO_EXCP, pre_xfam_14, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_UINT_PD, NO_EXCP, pre_xfam_14, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_UINT_PD, NO_EXCP, pre_xfam_14, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_UINT_TT, NO_EXCP, pre_xfam_14, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_UINT_TT, NO_EXCP, pre_xfam_14, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_DEBUG_INTERFACE, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(IA32_DEBUG_INTERFACE, X86_TRAP_VE, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_BNDCFGS, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_BNDCFGS, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_PASID, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_IA32_PASID, X86_TRAP_GP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_IA32_XSS, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),

	DEF_READ_MSR_SIZE(MSR_ARCH_LBR_INFO_0, NO_EXCP, pre_xfam_15, 0x100, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(MSR_ARCH_LBR_INFO_0, NO_EXCP, pre_xfam_15, 0x100, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_ARCH_LBR_CTL, NO_EXCP, pre_xfam_15, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_ARCH_LBR_CTL, NO_EXCP, pre_xfam_15, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_ARCH_LBR_DEPTH, NO_EXCP, pre_xfam_15, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR(MSR_ARCH_LBR_DEPTH, NO_EXCP, pre_xfam_15, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR_SIZE(MSR_ARCH_LBR_FROM_0, NO_EXCP, pre_xfam_15, 0x100, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(MSR_ARCH_LBR_FROM_0, NO_EXCP, pre_xfam_15, 0x100, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR_SIZE(MSR_ARCH_LBR_TO_0, NO_EXCP, pre_xfam_15, 0x100, VER1_0 | VER1_5 | VER2_0),
	DEF_WRITE_MSR_SIZE(MSR_ARCH_LBR_TO_0, NO_EXCP, pre_xfam_15, 0x100, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(IA32_UARCH_MISC_CTL, NO_EXCP, NO_PRE_COND, VER1_5),
	DEF_WRITE_MSR(IA32_UARCH_MISC_CTL, NO_EXCP, NO_PRE_COND, VER1_5),
	/* cases below here in WRMSR will crash the guest TD. */
	DEF_READ_MSR(MSR_EFER, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_STAR, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_LSTAR, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_CSTAR, X86_TRAP_VE, NO_PRE_COND, VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_SYSCALL_MASK, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_FS_BASE, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_GS_BASE, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_KERNEL_GS_BASE, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
	DEF_READ_MSR(MSR_TSC_AUX, NO_EXCP, NO_PRE_COND, VER1_0 | VER1_5 | VER2_0),
};
