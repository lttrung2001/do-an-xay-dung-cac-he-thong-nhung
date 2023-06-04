import numpy as np
import cv2
from camera_calibration import calib, undistort
from threshold import get_combined_gradients, get_combined_hls, combine_grad_hls
from line import Line, get_perspective_transform, get_lane_lines_img, illustrate_driving_lane, illustrate_info_panel, illustrate_driving_lane_with_topdownview


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
#       Select desired input name/type          #
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
input_type = 'image'
# input_type = 'video' 
#input_type = 'frame_by_frame'

input_name = 'test_images/test6/z4401856280431_b621b81382bd9b97df430fba657c1846.jpg'

left_line = Line()
right_line = Line()

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
#   Tune Parameters for different inputs        #
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
th_sobelx, th_sobely, th_mag, th_dir = (35, 100), (30, 255), (30, 255), (0.7, 1.3)
th_h, th_l, th_s = (10, 100), (0, 60), (85, 255)

# camera matrix & distortion coefficient
mtx, dist = calib()


def pipeline(frame):
    # Correcting for Distortion
    # Khử biến dạng (cong) cho ảnh
    undist_img = undistort(frame, mtx, dist)
    
    # resize image
    # Giảm độ phân giải ảnh xuống 1/2 ảnh gốc
    undist_img = cv2.resize(undist_img, None, fx=1 / 2, fy=1 / 2, interpolation=cv2.INTER_AREA)
    cv2.imshow("undist_img", undist_img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
    # Lấy kích thước ảnh theo chiều ngang và dọc theo pixel lưu vào biến rows và cols
    rows, cols = undist_img.shape[:2]

    # Detect edge và kết hợp các kết quả detect edge
    combined_gradient = get_combined_gradients(undist_img, th_sobelx, th_sobely, th_mag, th_dir)
    cv2.imshow("combined_gradient", combined_gradient)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    # Chuyển đổi ảnh sang hệ màu HLS và tách thành từng kênh màu riêng để tạo thành ảnh nhị phân. Sau đó kết hợp lại để ra ảnh cuối cùng
    combined_hls = get_combined_hls(undist_img, th_h, th_l, th_s)
    cv2.imshow("combined_hls", combined_hls)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    # Kết hợp gradient result và hls result
    combined_result = combine_grad_hls(combined_gradient, combined_hls)
    cv2.imshow("combined_result", combined_result)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    # Lấy chiều ngang và chiều dọc của ảnh
    c_rows, c_cols = combined_result.shape[:2]

    s_LTop2, s_RTop2 = [c_cols / 2 - 24, 5], [c_cols / 2 + 24, 5]
    s_LBot2, s_RBot2 = [110, c_rows], [c_cols - 110, c_rows]

    src = np.float32([s_LBot2, s_LTop2, s_RTop2, s_RBot2])
    dst = np.float32([(170, 720), (170, 0), (550, 0), (550, 720)])

    # Chuyển đổi view hiện tại thành bird eye view (nhìn từ trên xuống)
    warp_img, M, Minv = get_perspective_transform(combined_result, src, dst, (720, 720))
    cv2.imshow("warp_img", warp_img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    # Áp dụng kỹ thuật sliding window search để follow lane lines
    # Sau đó dựa vào các pixels trung tâm ta có thể tính được độ cong của làn đường
    searching_img = get_lane_lines_img(warp_img, left_line, right_line)
    cv2.imshow("searching_img", searching_img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    # w_comb_result, w_color_result = illustrate_driving_lane(searching_img, left_line, right_line)

    # # Drawing the lines back down onto the road
    # color_result = cv2.warpPerspective(w_color_result, Minv, (c_cols, c_rows))
    # lane_color = np.zeros_like(undist_img)
    # lane_color[220:rows - 12, 0:cols] = color_result

    # # Combine the result with the original image
    # result = cv2.addWeighted(undist_img, 1, lane_color, 0.3, 0)

    # info_panel, birdeye_view_panel = np.zeros_like(result),  np.zeros_like(result)
    # info_panel[5:110, 5:325] = (255, 255, 255)
    # birdeye_view_panel[5:110, cols-111:cols-6] = (255, 255, 255)
    
    # info_panel = cv2.addWeighted(result, 1, info_panel, 0.2, 0)
    # birdeye_view_panel = cv2.addWeighted(info_panel, 1, birdeye_view_panel, 0.2, 0)
    # road_map = illustrate_driving_lane_with_topdownview(w_color_result, left_line, right_line)
    # birdeye_view_panel[10:105, cols-106:cols-11] = road_map
    # birdeye_view_panel, int_direction = illustrate_info_panel(birdeye_view_panel, left_line, right_line)
    birdeye_view_panel, int_direction = illustrate_info_panel(undist_img, left_line, right_line)
    
    return birdeye_view_panel, int_direction


if __name__ == '__main__':
    # If working with images, don't use moviepy
    if input_type == 'image':
        img = cv2.imread(input_name)
        bird_eye, int_direction = pipeline(img)
        print(int_direction)
        cv2.imshow("test", bird_eye)
        cv2.waitKey(0)


