#ifndef __printfield_h__
#define __printfield_h__


#define PFF_TYPE_NUMERIC			0x00000000
#define	PFF_TYPE_TEXT				0x00000001
#define PFF_TYPE_DATE				0x00000002
#define PFF_TYPE_RESERVED			0x00000003
#define PFF_TYPE_MASKS				0x0000000F

#define PFF_TEXT_STYLE_INITBYDATE	0x00000010
#define PFF_TEXT_STYLE_MULTYLINE	0x00000020
#define PFF_TEXT_STYLE_COMBOBOX		0x00000040
#define PFF_TEXT_STYLE_COMBOBOX2	0x00000080
#define PFF_TEXT_STYLE_MASK			0x000000F0

#define PFF_NUMERIC_TYPE_INTEGER	0x00000000
#define PFF_NUMERIC_TYPE_DOUBLE		0x00000100
#define PFF_NUMERIC_TYPE_MASK		0x00000F00

#define PFF_NUMERIC_STYLE_DEFAULT	0x00001000
#define PFF_NUMERIC_STYLE_RANGES	0x00002000
#define PFF_NUMERIC_STYLE_MASK		0x0000F000

#define PFF_STYLE_ALIGN_L		0x00000000
#define PFF_STYLE_ALIGN_C		0x00010000
#define PFF_STYLE_ALIGN_R		0x00020000



struct print_field_params
{
	unsigned	flags;
	double	fmin, fmax, fdef;
	long	lmin, lmax, ldef;
};

enum PrintFieldAligment
{
	pfa_left = 0,
	pfa_center = 1,
	pfa_right = 2,
	pfa_width = -1,
};

interface IPrintField : public IUnknown
{
	com_call GetParams( print_field_params * pparams ) = 0;
	com_call SetParams( print_field_params * pparams ) = 0;
	com_call GetNames( BSTR *pbstrCaption, BSTR *pbstrGroup ) = 0;
	com_call GetCaptionAlingment( long *plAligment/*PrintFieldAligment enum*/ ) = 0;
	com_call GetCaptionWidth( long *plWidth/*if aligment == pfa_width*/ ) = 0;
};

declare_interface(IPrintField, "5EB313BA-42AE-4d0d-8C5B-16B4D6ADF3AF")


#endif //__printfield_h__