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
    cv2.imwrite("output_images/" + "original.jpg", frame)
    cv2.imshow("original", frame)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
    # Correcting for Distortion
    # Khử biến dạng (cong) cho ảnh
    # undist_img = undistort(frame, mtx, dist)
    # cv2.imshow("undist_img", undist_img)
    # cv2.waitKey(0)
    # cv2.destroyAllWindows()
    undist_img = frame

    # Lấy kích thước ảnh theo chiều ngang và dọc theo pixel lưu vào biến rows và cols
    rows, cols = undist_img.shape[:2]

    # Detect edge và kết hợp các kết quả detect edge
    combined_gradient = get_combined_gradients(undist_img, th_sobelx, th_sobely, th_mag, th_dir)
    cv2.imwrite("output_images/" + "combined_gradient.jpg", combined_gradient)
    cv2.imshow("combined_gradient", combined_gradient)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    # Chuyển đổi ảnh sang hệ màu HLS và tách thành từng kênh màu riêng để tạo thành ảnh nhị phân. Sau đó kết hợp lại để ra ảnh cuối cùng
    combined_hls = get_combined_hls(undist_img, th_h, th_l, th_s)
    cv2.imwrite("output_images/" + "combined_hls.jpg", combined_hls)
    cv2.imshow("combined_hls", combined_hls)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    # Kết hợp gradient result và hls result
    combined_result = combine_grad_hls(combined_gradient, combined_hls)
    cv2.imwrite("output_images/" + "combined_result.jpg", combined_result)
    cv2.imshow("combined_result", combined_result)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    # Lấy chiều ngang và chiều dọc của ảnh
    c_rows, c_cols = combined_result.shape[:2]

    # s_LTop2, s_RTop2 = [c_cols - 24, 5], [c_cols / 2 + 24, 5]
    # s_LBot2, s_RBot2 = [0, c_rows], [c_cols - 0, c_rows]
    s_LTop2, s_RTop2 = [0, 320], [408, 320]
    s_LBot2, s_RBot2 = [0, 480], [408, 480]

    src = np.float32([s_LBot2, s_LTop2, s_RTop2, s_RBot2])
    # left bottom, left top, right top, right bottom
    dst = np.float32([(100 - 44, 480), (0, 320), (408, 320), (308 + 44, 480)])

    

    # Chuyển đổi view hiện tại thành bird eye view (nhìn từ trên xuống)
    warp_img, M, Minv = get_perspective_transform(combined_result, src, dst, (480, 480))
    cv2.imwrite("output_images/" + "warp_img.jpg", warp_img)
    cv2.imshow("warp_img", warp_img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    # Áp dụng kỹ thuật sliding window search để follow lane lines
    # Sau đó dựa vào các pixels trung tâm ta có thể tính được độ cong của làn đường
    searching_img = get_lane_lines_img(warp_img, left_line, right_line)
    cv2.imwrite("output_images/" + "searching_img.jpg", searching_img)
    cv2.imshow("searching_img", searching_img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

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


