# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2024 Vincent Fazio <vfazio@gmail.com>

def set_process_name(name: str) -> None: ...
def check_kernel_version(major: int, minor: int, release: int) -> bool: ...
