package com.example.mads.lightblu;

public class TypeManufacturerData extends AdElement {

	public TypeManufacturerData( byte data[],int pos, int len) {
		int ptr = pos;
		int v = ((int)data[ptr]) & 0xFF;
		ptr++;
		v |= (((int)data[ptr]) & 0xFF ) << 8;
		ptr++;
		manufacturer = v;
		int blen = len - 2;
		b = new byte[blen];
		System.arraycopy(data, ptr, b, 0, blen);
    }


	
	@Override
	public String toString() {
		StringBuffer sb = new StringBuffer( "Lysstatus: ");

        for( int i = 0 ; i < b.length ; ++i ) {
			if( i > 0)
				sb.append("");
			int v = ((int)b[i]) & 0xFF;
			if (v == 0x05)
                sb.append("PÅ");
            if (v == 0x07)
                sb.append("AV");
            if (v == 0x0A)
                sb.append("0%");
            if (v == 0x0C)
                sb.append("20%");
            if (v == 0x0E)
                sb.append("40%");
            if (v == 0x10)
                sb.append("60%");
            if (v == 0x12)
                sb.append("80%");
            if (v == 0x14)
                sb.append("100%");


            Buffer.lysadv = v;

		}


        return new String(sb);

	}

	public int getManufacturer() {
		return manufacturer;
	}
	
	public byte[] getBytes() {
		return b;
	}
	
	int manufacturer;
	byte b[];

}
