/*  =========================================================================
    fty_common_socket_helpers - Helper functions for communication

    Copyright (C) 2014 - 2019 Eaton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    =========================================================================
*/

/*
@header
    fty_common_socket_helpers - Helper functions for communication
@discuss
@end
*/

#include "fty_common_socket_helpers.h"


#include <unistd.h>
#include <stdexcept>

#include <iostream>

namespace fty
{

    Payload recvFrames(int socket)
    {
        //format => [ Number of frames ], [ <size of frame 1> <data> ], ... [ <size of frame N> <data> ]

        //get the number of frames
        uint32_t numberOfFrame = 0;

        std::cerr << "reading data..." << std::endl;
        
        if(read(socket, &numberOfFrame, sizeof(uint32_t)) != sizeof(uint32_t))
        {
            throw std::runtime_error("Error while reading number of frame");
        }
        
        std::cerr << "reading "<< numberOfFrame <<" frames ..." << std::endl;

        //Get frames
        Payload frames;

        for( uint32_t index = 0; index < numberOfFrame; index++)
        {
            //get the size of the frame
            uint32_t frameSize = 0;
            
            std::cerr << "reading size of frame "<< index <<" ..." << std::endl;
            
            if(read(socket, &frameSize, sizeof(uint32_t)) != sizeof(uint32_t))
            {
                throw std::runtime_error("Error while reading size of frame");
            }
            
            std::cerr << "reading payload of "<< frameSize <<" bytes of frame "<< index <<"..." << std::endl;

            if(frameSize == 0)
            {
                throw std::runtime_error("Read error: Empty frame");
            }

            //get the payload of the frame
            std::vector<char> buffer;
            buffer.reserve(frameSize+1);
            buffer[frameSize] = 0;

            if(read(socket, &buffer[0], frameSize) != frameSize)
            {
                throw std::runtime_error("Read error while getting payload of frame");
            }
            
            std::string str(&buffer[0]);

            std::cerr << "Payload of frame "<<index <<": "<< str << std::endl;
            
            frames.push_back(str);
        }
        
        return frames;
    }
    
    void sendFrames(int socket, const Payload & payload)
    {
        //Send number of frame
        uint32_t numberOfFrame = payload.size();
        
        if ( write(socket, &numberOfFrame, sizeof(uint32_t)) != sizeof(uint32_t) )
        {
            throw std::runtime_error("Error while writing number of frame");
        }
        
        for(const std::string & frame : payload)
        {
            uint32_t frameSize = frame.length() + 1;
            
            if ( write(socket, &frameSize, sizeof(uint32_t)) != sizeof(uint32_t) )
            {
                throw std::runtime_error("Error while writing size of frame");
            }
            
            if ( write(socket, frame.data(), frameSize) != frameSize )
            {
                throw std::runtime_error("Error while writing payload");
            }    
        }
    }
    
} //namespace fty
