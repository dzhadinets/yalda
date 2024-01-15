#!/bin/bash

# Get value in kernel .config from key
function kernel_get_config_value
{
  local CONFIG_KEY=$1
  grep $CONFIG_KEY $KERNEL_CONFIG_PATH | cut -d= -f2
}

function arch_x86_64_get_paging_done_addr
{
  # We want to find address of kernel instruction in startup procedure, which enables final
  # version of page table.
  # Page table setup is done in the very first steps when kernel code is run.
  # For 64 bit kernel, first run instruction already runs in paging mode, but uses identity page
  # table, meaning phyisical and virtual addresses match.
  # start. So
  # - kernel is uncompressed to 0x01000000 and first instruction is run
  # - startup_64 arch/x86/kernel/head_64.S is the first instruction symbol
  # - startup_64 runs __startup_64 in arch/x86/kernel/head64.c
  # - __startup_64 sets up final page table and returns to startup_64
  # - startup64 jumps to secondary_startup_64_no_verify
  # -secondary_startup_64_no_verify changes value page table based address in CR3 register
  # -then it flushes TLB and paging setup is considered done
  #
  # The code for this is considered more or less static and probably will not change,
  # Below is a snippet that we look for. We want to know the last address of this snippet
  # What happens:
  # cr3 register receives base address of the new page table
  # rcx, rax receives contents of cr4 register, one with paging 'on', one with paging 'off'
  # cr4 is then updated twice to quickly turn off, then turn on paging. This way TLB gets
  # flushed and new address translation requests will be calculated from scratch on the new
  # page table
  # This is the point where we can set breakpoints on virtual addresses and it will work
  
  #	/*
  #	 * Switch to new page-table
  #	 *
  #	 * For the boot CPU this switches to early_top_pgt which still has the
  #	 * indentity mappings present. The secondary CPUs will switch to the
  #	 * init_top_pgt here, away from the trampoline_pgd and unmap the
  #	 * indentity mapped ranges.
  #	 */
  #	movq	%rax, %cr3
  #
  #	/*
  #	 * Do a global TLB flush after the CR3 switch to make sure the TLB
  #	 * entries from the identity mapping are flushed.
  #	 */
  #	movq	%cr4, %rcx
  #	movq	%rcx, %rax
  #	xorq	$X86_CR4_PGE, %rcx
  #	movq	%rcx, %cr4
  #	movq	%rax, %cr4
  # Note: code snippet taken from linux kernel version 5.19 and tested on it.
  # Enabling paging is a very standard system procedure, so it can not have
  # too much different implementations. It is pretty safe to assume this will
  # not change much in newer kernel versions

  OBJFILE=$KERNEL_BUILD_DIR/arch/x86/kernel/head_64.o

  CHECK_LINE=$(objdump -d $OBJFILE | grep -A 5 'mov.*ax.*cr3' | tail -1)

  if ! echo $CHECK_LINE | grep --silent cr4; then
    log_error "Failed to detect instruction block that enables page table"
    return -1
  fi

  FULL_LINE=$(objdump -d $OBJFILE | grep -A 6 'mov.*ax.*cr3' | tail -1)
  INSTRUCTION_OFFSET=$(echo $FULL_LINE | cut -d: -f1)
  if [ -z "$INSTRUCTION_OFFSET" ]; then
    log_error "Failed to read address offset from objdump"
    return -1
  fi

  INSTRUCTION_OFFSET=$(echo "0x${INSTRUCTION_OFFSET}")

  CONFIG_PHYSICAL_START=$(kernel_get_config_value CONFIG_PHYSICAL_START)
  if [ -z "$CONFIG_PHYSICAL_START" ]; then
    log_error "Failed to read value from kernel config"
    return -1
  fi

  printf "0x%08x" $(($CONFIG_PHYSICAL_START + $INSTRUCTION_OFFSET))
}

function arch_x86_get_paging_done_addr
{
  OBJFILE=$KERNEL_BUILD_DIR/arch/x86/kernel/head_32.o

  CHECK_LINE=$(objdump -d $OBJFILE | grep -A 2 'mov.*ax.*cr3' | tail -1)

  if ! echo $CHECK_LINE | grep --silent cr0; then
    log_error "Failed to detect instruction block that enables page table"
    return -1
  fi

  FULL_LINE=$(objdump -d $OBJFILE | grep -A 3 'mov.*ax.*cr3' | tail -1)
  INSTRUCTION_OFFSET=$(echo $FULL_LINE | cut -d: -f1)
  if [ -z "$INSTRUCTION_OFFSET" ]; then
    log_error "Failed to read address offset from objdump"
    return -1
  fi

  INSTRUCTION_OFFSET=$(echo "0x${INSTRUCTION_OFFSET}")

  CONFIG_PHYSICAL_START=$(kernel_get_config_value CONFIG_PHYSICAL_START)
  if [ -z "$CONFIG_PHYSICAL_START" ]; then
    log_error "Failed to read value from kernel config"
    return -1
  fi

  printf "0x%08x" $(($CONFIG_PHYSICAL_START + $INSTRUCTION_OFFSET))
}

if [ "$#" != "3" ]; then
  log_error "Incorrect number of arguments"
  exit -1
fi

ARCH=$1
KERNEL_BUILD_DIR=$2
KERNEL_CONFIG_PATH=$3

if [ "$ARCH" == "x86_64" ]; then
  arch_x86_64_get_paging_done_addr
else
  arch_x86_get_paging_done_addr
fi
