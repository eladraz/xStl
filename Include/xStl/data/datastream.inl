/*
 * Copyright (c) 2008-2016, Integrity Project Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of the Integrity Project nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE
 */

/*
 * datastream.inl
 *
 * Implementation of template file.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
template <class OSTREAM, class FW_ITR, class DT>
OSTREAM & hexDumpObject(OSTREAM &out, DUMP<FW_ITR, DT>& object)
{
    addressNumericValue address = object.m_startAddress; /* The start address */
	FW_ITR index = object.m_begin;                /* The position in the scan */
	unsigned int element = 0;               /* The element position in a line */
	unsigned int objectLocation = 0;/* The position in the object that dumped */
	const unsigned int objectSize = sizeof(DT);       /* size of each element */
	cBuffer data(object.m_elementsInRow);  /* The char display of data at end */

	while (index != object.m_end)
	{
		/* Test whether we need to write the address */
		if (element == 0)
		{
			if (object.m_segmentName != NULL)
			{
				// Print the segment name
				out << object.m_segmentName << ":";
			}
            if (object.m_flags & DUMP<FW_ITR, DT>::DATA_USE_ADDRESS)
			{
                out << HEXADDRESS(address) << "   ";
			}
		}

		/* Write the element */
		unsigned char db = *((unsigned char *)(&(*index)) + objectLocation); /* The element in the queue*/
		data[element] = DB(db); // Save the data
		out << (char)(HEXHIGH(db)) << (char)(HEXLOW(db));
		if ((element & 1) != 0)
			out << " ";

		/*
		 * Only if the inner index location is printed
		 * Then we go to the next element
		 */
		objectLocation++;
		if (objectLocation == objectSize)
		{
			objectLocation = 0;
			index++;
		}
		address++;
		element = (element + 1) % object.m_elementsInRow;

		/* Test for new line */
		if (element == 0)
		{
			/* Flash data */
			for (unsigned int i = 0; i < object.m_elementsInRow; i++)
			{
				out << (char)(data[i]);
			}
			out << endl;
		}
	}

	/* Flash data if needed */
	if (element != 0)
	{
		unsigned int i;
		for (i = 0; i < (object.m_elementsInRow - element); i++)
		{
			out << "  ";
			if (((i + element) & 1) != 0)
				out << " ";
		}
		for (i = 0; i < element; i++)
		{
			out << (char)(data[i]);
		}
		out << endl;
	}

	return out;
}

template <class OSTREAM, class FW_ITR, class DT>
OSTREAM& operator << (OSTREAM &out, DUMP<FW_ITR, DT>& object)
{
    return hexDumpObject(out, object);
}
