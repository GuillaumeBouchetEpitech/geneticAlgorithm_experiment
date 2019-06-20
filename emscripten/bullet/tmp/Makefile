

# MY_VAR_MODE=no

# ifeq ($(MY_VAR_MODE),yes)

# 	MY_VAR_VALUE="IS YES"

# else

# 	MY_VAR_VALUE="IS NO"

# endif



# my_var:
# 	@$(eval MY_VAR_MODE=yes)
# 	@echo "MY_VAR_VALUE=$(MY_VAR_VALUE)"
# 	@echo "MY_VAR_MODE=$(MY_VAR_MODE)"

# my_var2:
# 	@$(eval $@MY_VAR_MODE=yes)
# 	@echo "MY_VAR_VALUE=$(MY_VAR_VALUE)"
# 	@echo "MY_VAR_MODE=$(MY_VAR_MODE)"

# my_var2:
# 	@$(eval MY_VAR_MODE=yes)
# 	@$(MAKE my_var)


MY_VAR_MODE=no

ifeq ($(MY_VAR_MODE),yes)


my_var:
	@echo "IS YES"


else


my_var:
	@echo "IS NO"


endif
