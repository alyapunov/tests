/**
 * MP_EXT is printed as "EXT" only, all MP_EXT data is skipped
 */
static inline int
mp_print_internal(const char **beg)
{
	switch (mp_typeof(**beg)) {
	case MP_NIL:
		mp_next(beg);
		printf("NIL");
		break;
	case MP_UINT:
		printf("%" PRIu64, mp_decode_uint(beg));
		break;
	case MP_INT:
		printf("%" PRId64, mp_decode_int(beg));
		break;
	case MP_STR:
	{
		uint32_t strlen;
		const char *str = mp_decode_str(beg, &strlen);
		printf("\"%.*s\"", strlen, str);
		break;
	}
	case MP_BIN:
	{
		uint32_t binlen;
		const char *bin = mp_decode_bin(beg, &binlen);
		printf("(");
		const char *hex = "0123456789ABCDEF";
		for (uint32_t i = 0; i < binlen; i++) {
			unsigned char c = (unsigned char)bin[i];
			printf("%c%c", hex[c >> 4], hex[c & 0xF]);
		}
		printf(")");
		break;
	}
	case MP_ARRAY:
	{
		uint32_t size = mp_decode_array(beg);
		printf("[");
		for (uint32_t i = 0; i < size; i++) {
			if (i)
				printf(", ");
			mp_print_internal(beg);
		}
		printf("]");
		break;
	}
	case MP_MAP:
	{
		uint32_t size = mp_decode_map(beg);
		printf("{");
		for (uint32_t i = 0; i < size; i++) {
			if (i)
				printf(", ");
			mp_print_internal(beg);
			printf(":");
			mp_print_internal(beg);
		}
		printf("}");
		break;
	}
	case MP_BOOL:
		printf("%s", mp_decode_bool(beg) ? "true" : "false");
		break;
	case MP_FLOAT:
		printf("%g", mp_decode_float(beg));
		break;
	case MP_DOUBLE:
		printf("%lg", mp_decode_double(beg));
		break;
	case MP_EXT:
		mp_next(beg);
		printf("EXT");
		break;
	default:
		assert(false);
		return -1;
	}
	return 0;
}

/**
 * MP_EXT is printed as "EXT" only, all MP_EXT data is skipped
 */
static inline int
mp_print(const char *beg)
{
	int res = mp_print_internal(&beg);
	printf("\n");
	return res;
}

