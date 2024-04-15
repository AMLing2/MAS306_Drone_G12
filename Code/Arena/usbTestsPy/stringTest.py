import usb.core

printers = usb.core.find(find_all=True, bDeviceClass=2)
print(f'There are {len(printers)} in the system')
